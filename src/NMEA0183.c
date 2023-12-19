#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "NMEA0183.h"

NMEA0183_t *NMEA0183_Init()
{

    NMEA0183_t *nmea0183 = (NMEA0183_t *)malloc(sizeof(NMEA0183_t));

    if (nmea0183 == NULL)
    {
        return NULL;
    }

    nmea0183->sentence = NMEA0183_InitSentence();
    nmea0183->comunication = NMEA0183_InitComunication();

    nmea0183->numRegisteredSensors = 0;
    for (int i = 0; i < MAX_SENSORS; i++)
    {
        nmea0183->registeredSensors[i] = NULL;
    }

    if (nmea0183->sentence == NULL ||
        nmea0183->comunication == NULL)
    {
        free(nmea0183);
        return NULL;
    }

    return nmea0183;
}

sentence_t *NMEA0183_InitSentence()
{
    sentence_t *sentence = (sentence_t *)malloc(sizeof(sentence_t));

    if (sentence != NULL)
    {
        for (size_t i = 0; i < MAX_FIELDS_COUNT; i++)
        {
            sentence->fields[i] = (char *)malloc(10 * sizeof(char));
        }

        if (sentence->fields != NULL)
        {
            NMEA0183_Reset_Sentence(sentence);
        }

        return sentence;
    }

    return NULL;
}

comunication_t *NMEA0183_InitComunication()
{
    comunication_t *comunication = (comunication_t *)malloc(sizeof(comunication_t));

    if (comunication != NULL)
    {
        NMEA0183_Reset_Comunication(comunication);
        return comunication;
    }

    return NULL;
}

void NMEA0183_Reset(NMEA0183_t *nmea0183)
{
    NMEA0183_Reset_Sentence(nmea0183->sentence);
    NMEA0183_Reset_Comunication(nmea0183->comunication);

    for (uint8_t i = 0; i < nmea0183->numRegisteredSensors; i++)
    {
        registered_sensor_t *sensor = nmea0183->registeredSensors[i];
        sensor->resetterFunction(sensor->data);
    }
}

void NMEA0183_Reset_Sentence(sentence_t *sentence)
{
    sentence->length = 0;
    memset(sentence->buffer, 0, SENTENCE_MAX_LEGTH);
    sentence->talkerID = UNKNOWN;
    sentence->numberOfFields = 0;
    for (size_t i = 0; i < MAX_FIELDS_COUNT; i++)
    {
        memset(sentence->fields[i], 0, 10);
    }
    sentence->checksum = 0;
    sentence->isChecksumValid = false;
}

void NMEA0183_Reset_Comunication(comunication_t *comunication)
{
    comunication->stage = COMUNICATION_STAGE_WAITING;
    comunication->status = COMUNICATION_STATUS_OK;
}

void NMEA0183_RegisterSensor(NMEA0183_t *nmea0183, registered_sensor_t sensor_value)
{
    registered_sensor_t *sensor_pointer = (registered_sensor_t *)malloc(sizeof(registered_sensor_t));

    if (sensor_pointer == NULL)
    {
        fprintf(stderr, "Memory allocation failed for sensor registration.\n");
        return;
    }

    sensor_pointer->ID = sensor_value.ID;
    sensor_pointer->parserFunction = sensor_value.parserFunction;
    sensor_pointer->printerFunction = sensor_value.printerFunction;
    sensor_pointer->resetterFunction = sensor_value.resetterFunction;
    sensor_pointer->data = sensor_value.data;

    nmea0183->registeredSensors[nmea0183->numRegisteredSensors++] = sensor_pointer;
}

void NMEA0183_Update(NMEA0183_t *nmea0183, uint8_t c)
{

    switch (nmea0183->comunication->stage)
    {
    case COMUNICATION_STAGE_WAITING:
        if (c == SENTENCE_CHARACTER_START)
        {
            nmea0183->comunication->stage = COMUNICATION_STAGE_INCOMING;
            NMEA0183_AddToBuffer(nmea0183, c);
        }
        break;

    case COMUNICATION_STAGE_INCOMING:
        NMEA0183_AddToBuffer(nmea0183, c);
        if (c == SENTENCE_CHARACTER_END ||
            c == SENTENCE_CHARACTER_END_CR ||
            c == SENTENCE_CHARACTER_END_LF)
        {
            nmea0183->comunication->stage = COMUNICATION_STAGE_ANALYZING;
            NMEA0183_AnalyzeData(nmea0183);
            printf("ID: %d\n", nmea0183->sentence->talkerID);
            if (nmea0183->comunication->status == COMUNICATION_STATUS_ERROR)
            {
                break;
            }

            nmea0183->comunication->stage = COMUNICATION_STAGE_DONE;
            // NMEA0183_PrintData(nmea0183);
            NMEA0183_Reset(nmea0183);
        }

    case COMUNICATION_STAGE_DONE:
        break;

    default:
        NMEA0183_Reset(nmea0183);
        break;
    }

    if (nmea0183->comunication->status == COMUNICATION_STATUS_ERROR)
    {
        NMEA0183_Reset(nmea0183);
    }
}

void NMEA0183_AddToBuffer(NMEA0183_t *nmea0183, uint8_t c)
{
    if (nmea0183->sentence->length >= SENTENCE_MAX_LEGTH)
    {
        nmea0183->comunication->status = COMUNICATION_STATUS_ERROR;
        return;
    }

    nmea0183->sentence->buffer[nmea0183->sentence->length] = c;
    nmea0183->sentence->length++;
}

void NMEA0183_AnalyzeData(NMEA0183_t *nmea0183)
{

    if (nmea0183->sentence->buffer[0] != SENTENCE_CHARACTER_START)
    {
        nmea0183->comunication->status = COMUNICATION_STATUS_ERROR;
        return;
    }

    if (nmea0183->sentence->buffer[nmea0183->sentence->length - 1] != SENTENCE_CHARACTER_END &&
        nmea0183->sentence->buffer[nmea0183->sentence->length - 1] != SENTENCE_CHARACTER_END_LF &&
        nmea0183->sentence->buffer[nmea0183->sentence->length - 1] != SENTENCE_CHARACTER_END_CR)
    {
        nmea0183->comunication->status = COMUNICATION_STATUS_ERROR;
        return;
    }

    if (nmea0183->sentence->buffer[nmea0183->sentence->length - 4] != SENTENCE_CHARACTER_CHECKSUM_START)
    {
        nmea0183->comunication->status = COMUNICATION_STATUS_ERROR;
        return;
    }

    NMEA0183_GetTalkerID(nmea0183);
    NMEA0183_GetFields(nmea0183);
    NMEA0183_GetChecksum(nmea0183);

    nmea0183->sentence->isChecksumValid = (nmea0183->sentence->checksum == NMEA0183_ComputeChecksum(nmea0183->sentence->buffer, nmea0183->sentence->length));
    if (nmea0183->sentence->isChecksumValid == false)
    {
        nmea0183->comunication->status = COMUNICATION_STATUS_ERROR;
        return;
    }

    NMEA0183_ParseData(nmea0183);
}

void NMEA0183_GetTalkerID(NMEA0183_t *nmea0183)
{
    sensor_association_t associations[] = {
        {"IIMWV", MWV},
        {"WIXDR", XDR},
        {NULL, UNKNOWN}};

    char talkerID[10] = {0};

    uint8_t i = 1; // Skip over the $ at the begining of the sentence
    while (nmea0183->sentence->buffer[i] != SENTENCE_CHARACTER_DELIMITER && i < nmea0183->sentence->length)
    {
        talkerID[i - 1] = nmea0183->sentence->buffer[i];
        i++;
    }

    for (i = 0; associations[i].sensorTypeString != NULL; i++)
    {
        if (strcmp(talkerID, associations[i].sensorTypeString) == 0)
        {
            nmea0183->sentence->talkerID = associations[i].sensorType;
            return;
        }
    }

    nmea0183->sentence->talkerID = UNKNOWN;
}

void NMEA0183_GetFields(NMEA0183_t *nmea0183)
{
    nmea0183->sentence->numberOfFields = 0;
    uint8_t i = 0;
    uint8_t j = 0;
    char tmp[10] = {0};

    while (i < nmea0183->sentence->length &&
           nmea0183->sentence->buffer[i] != SENTENCE_CHARACTER_CHECKSUM_START &&
           nmea0183->sentence->buffer[i] != SENTENCE_CHARACTER_END &&
           nmea0183->sentence->buffer[i] != SENTENCE_CHARACTER_END_CR &&
           nmea0183->sentence->buffer[i] != SENTENCE_CHARACTER_END_LF)
    {
        if (nmea0183->sentence->buffer[i] != SENTENCE_CHARACTER_DELIMITER)
        {
            tmp[j] = nmea0183->sentence->buffer[i];
            j++;
        }
        else
        {
            tmp[j] = '\0';
            strncpy(nmea0183->sentence->fields[nmea0183->sentence->numberOfFields++], tmp, 10);

            memset(tmp, 0, SENTENCE_MAX_LEGTH);
            j = 0;
        }

        i++;
    }

    if (tmp[0] != '\0')
    {
        tmp[j] = '\0';
        strncpy(nmea0183->sentence->fields[nmea0183->sentence->numberOfFields++], tmp, 10);
    }
}

void NMEA0183_GetChecksum(NMEA0183_t *nmea0183)
{
    nmea0183->sentence->checksum = (uint8_t)strtol(&nmea0183->sentence->buffer[nmea0183->sentence->length - 3], NULL, 16);
}

uint8_t NMEA0183_ComputeChecksum(char *buffer, uint8_t length)
{
    uint8_t checksum_value = 0;

    uint8_t i = 1; // To skip over the $ at the begining of the sentence
    while (i < length && buffer[i] != SENTENCE_CHARACTER_CHECKSUM_START)
    {
        checksum_value ^= buffer[i];
        i++;
    }

    return checksum_value;
}

void NMEA0183_ParseData(NMEA0183_t *nmea0183)
{
    sensor_t sensorType = nmea0183->sentence->talkerID;

    for (uint8_t i = 0; i < nmea0183->numRegisteredSensors; i++)
    {
        registered_sensor_t *sensor = nmea0183->registeredSensors[i];

        if (sensor->ID == sensorType)
        {
            sensor->parserFunction(nmea0183->sentence->fields, sensor->data);
            return;
        }
    }

    nmea0183->comunication->status = COMUNICATION_STATUS_ERROR;
}

void NMEA0183_PrintData(NMEA0183_t *nmea0183)
{

    sensor_t sensorType = nmea0183->sentence->talkerID;

    for (uint8_t i = 0; i < nmea0183->numRegisteredSensors; i++)
    {
        registered_sensor_t *sensor = nmea0183->registeredSensors[i];

        if (sensor->ID == sensorType)
        {
            sensor->printerFunction(sensor->data);
            return;
        }
    }

    nmea0183->comunication->status = COMUNICATION_STATUS_ERROR;
}