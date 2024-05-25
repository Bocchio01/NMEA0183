/**
 * @file NMEA0183.c
 * @brief NMEA0183 Protocol Handling Library Implementation
 *
 * @author Windex Team
 * @date 19/12/2023
 */

#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "NMEA0183.h"
#include "sensors.h"

NMEA0183_t *NMEA0183_Init()
{

    NMEA0183_t *nmea0183 = (NMEA0183_t *)malloc(sizeof(NMEA0183_t));

    if (nmea0183 == NULL)
    {
        return NULL;
    }

    nmea0183->sentence = NMEA0183_InitSentence();
    nmea0183->comunication = NMEA0183_InitComunication();
    nmea0183->registeredSensor = NMEA0183_InitRegisteredSensor();

    if (nmea0183->sentence != NULL &&
        nmea0183->comunication != NULL &&
        nmea0183->registeredSensor != NULL)
    {
        return nmea0183;
    }

    free(nmea0183);
    return NULL;
}

sentence_t *NMEA0183_InitSentence()
{
    sentence_t *sentence = (sentence_t *)malloc(sizeof(sentence_t));

    if (sentence != NULL)
    {
        sentence->buffer = (buffer_t *)malloc(sizeof(buffer_t));
        sentence->fields = (fields_t *)malloc(sizeof(fields_t));
        sentence->checksum = (checksum_t *)malloc(sizeof(checksum_t));
        if (sentence->buffer != NULL &&
            sentence->fields != NULL &&
            sentence->checksum != NULL)
        {
            NMEA0183_ResetSentence(sentence);
            return sentence;
        }
    }

    return NULL;
}

comunication_t *NMEA0183_InitComunication()
{
    comunication_t *comunication = (comunication_t *)malloc(sizeof(comunication_t));

    if (comunication != NULL)
    {
        NMEA0183_ResetComunication(comunication);
        return comunication;
    }

    return NULL;
}

registered_sensor_t *NMEA0183_InitRegisteredSensor()
{
    registered_sensor_t *registeredSensor = (registered_sensor_t *)malloc(sizeof(registered_sensor_t));

    if (registeredSensor != NULL)
    {
        registeredSensor->sizeOf = 0;
        NMEA0183_ResetRegisteredSensor(registeredSensor);
        return registeredSensor;
    }

    return NULL;
}

void NMEA0183_Reset(NMEA0183_t *nmea0183)
{
    NMEA0183_ResetSentence(nmea0183->sentence);
    NMEA0183_ResetComunication(nmea0183->comunication);
    NMEA0183_ResetRegisteredSensor(nmea0183->registeredSensor);
}

void NMEA0183_ResetSentence(sentence_t *sentence)
{
    sentence->buffer->sizeOf = 0;
    memset(sentence->buffer->data, 0, SENTENCE_MAX_LEGTH);
    sentence->sensorID = UNKNOWN;
    sentence->fields->sizeOf = 0;
    for (size_t i = 0; i < FIELDS_MAX_COUNT; i++)
    {
        memset(sentence->fields->data[i], 0, FIELD_MAX_LENGTH);
    }
    sentence->checksum->value = 0;
    sentence->checksum->isValid = false;
}

void NMEA0183_ResetComunication(comunication_t *comunication)
{
    comunication->stage = COMUNICATION_STAGE_WAITING;
    comunication->status = COMUNICATION_STATUS_OK;
}

void NMEA0183_ResetRegisteredSensor(registered_sensor_t *registeredSensor)
{
    for (uint8_t i = 0; i < registeredSensor->sizeOf; i++)
    {
        sensor_t *sensor = registeredSensor->sensor[i];
        sensor->resetterFunction(sensor->data);
    }
}

void NMEA0183_RegisterSensor(NMEA0183_t *nmea0183, sensor_t sensor_value)
{
    sensor_t *sensor_pointer = (sensor_t *)malloc(sizeof(sensor_t));

    if (sensor_pointer == NULL)
    {
        fprintf(stderr, "Memory allocation failed for sensor registration.\n");
        return;
    }

    sensor_pointer->sensorID = sensor_value.sensorID;
    sensor_pointer->parserFunction = sensor_value.parserFunction;
    sensor_pointer->printerFunction = sensor_value.printerFunction;
    sensor_pointer->resetterFunction = sensor_value.resetterFunction;
    sensor_pointer->data = sensor_value.data;

    nmea0183->registeredSensor->sensor[nmea0183->registeredSensor->sizeOf++] = sensor_pointer;
}

void NMEA0183_CharacterHandler(NMEA0183_t *nmea0183, uint8_t c)
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
            if (nmea0183->comunication->status == COMUNICATION_STATUS_ERROR)
            {
                break;
            }

            nmea0183->comunication->stage = COMUNICATION_STAGE_DONE;
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
    if (nmea0183->sentence->buffer->sizeOf >= SENTENCE_MAX_LEGTH)
    {
        nmea0183->comunication->status = COMUNICATION_STATUS_ERROR;
        return;
    }

    nmea0183->sentence->buffer->data[nmea0183->sentence->buffer->sizeOf] = c;
    nmea0183->sentence->buffer->sizeOf++;
}

void NMEA0183_AnalyzeData(NMEA0183_t *nmea0183)
{

    if (nmea0183->sentence->buffer->data[0] != SENTENCE_CHARACTER_START)
    {
        nmea0183->comunication->status = COMUNICATION_STATUS_ERROR;
        return;
    }

    if (nmea0183->sentence->buffer->data[nmea0183->sentence->buffer->sizeOf - 1] != SENTENCE_CHARACTER_END &&
        nmea0183->sentence->buffer->data[nmea0183->sentence->buffer->sizeOf - 1] != SENTENCE_CHARACTER_END_LF &&
        nmea0183->sentence->buffer->data[nmea0183->sentence->buffer->sizeOf - 1] != SENTENCE_CHARACTER_END_CR)
    {
        nmea0183->comunication->status = COMUNICATION_STATUS_ERROR;
        return;
    }

    if (nmea0183->sentence->buffer->data[nmea0183->sentence->buffer->sizeOf - 4] != SENTENCE_CHARACTER_CHECKSUM_START)
    {
        nmea0183->comunication->status = COMUNICATION_STATUS_ERROR;
        return;
    }

    NMEA0183_AnalyzeSensorID(nmea0183);
    if (nmea0183->sentence->sensorID == UNKNOWN)
    {
        nmea0183->comunication->status = COMUNICATION_STATUS_ERROR;
        return;
    }

    NMEA0183_AnalyzeFields(nmea0183);
    NMEA0183_AnalyzeChecksum(nmea0183);

    nmea0183->sentence->checksum->isValid = (nmea0183->sentence->checksum->value == NMEA0183_ComputeChecksum(nmea0183->sentence->buffer->data, nmea0183->sentence->buffer->sizeOf));
    if (nmea0183->sentence->checksum->isValid == false)
    {
        nmea0183->comunication->status = COMUNICATION_STATUS_ERROR;
        return;
    }

    NMEA0183_ParseData(nmea0183);
}

void NMEA0183_AnalyzeSensorID(NMEA0183_t *nmea0183)
{
    char sensorID[FIELD_MAX_LENGTH] = {0};
    uint8_t numberOfSensors = sizeof(sensorAssociation) / sizeof(sensorAssociation[0]);

    uint8_t i = 1; // Skip over the $ at the begining of the sentence
    while (nmea0183->sentence->buffer->data[i] != SENTENCE_CHARACTER_DELIMITER && i < nmea0183->sentence->buffer->sizeOf)
    {
        sensorID[i - 1] = nmea0183->sentence->buffer->data[i];
        i++;
    }

    for (i = 0; i < numberOfSensors; i++)
    {
        if (strcmp(sensorID, sensorAssociation[i].sensorTypeString) == 0)
        {
            nmea0183->sentence->sensorID = sensorAssociation[i].sensorType;
            return;
        }
    }

    nmea0183->sentence->sensorID = UNKNOWN;
}

void NMEA0183_AnalyzeFields(NMEA0183_t *nmea0183)
{
    nmea0183->sentence->fields->sizeOf = 0;
    uint8_t i = 0;
    uint8_t j = 0;
    char tmp[FIELD_MAX_LENGTH] = {0};

    while (i < nmea0183->sentence->buffer->sizeOf &&
           nmea0183->sentence->buffer->data[i] != SENTENCE_CHARACTER_CHECKSUM_START &&
           nmea0183->sentence->buffer->data[i] != SENTENCE_CHARACTER_END &&
           nmea0183->sentence->buffer->data[i] != SENTENCE_CHARACTER_END_CR &&
           nmea0183->sentence->buffer->data[i] != SENTENCE_CHARACTER_END_LF)
    {
        if (nmea0183->sentence->buffer->data[i] != SENTENCE_CHARACTER_DELIMITER)
        {
            tmp[j] = nmea0183->sentence->buffer->data[i];
            j++;
        }
        else
        {
            tmp[j] = '\0';
            strncpy(nmea0183->sentence->fields->data[nmea0183->sentence->fields->sizeOf++], tmp, FIELD_MAX_LENGTH);

            memset(tmp, 0, FIELD_MAX_LENGTH);
            j = 0;
        }

        i++;
    }

    if (tmp[0] != '\0')
    {
        tmp[j] = '\0';
        strncpy(nmea0183->sentence->fields->data[nmea0183->sentence->fields->sizeOf++], tmp, FIELD_MAX_LENGTH);
    }
}

void NMEA0183_AnalyzeChecksum(NMEA0183_t *nmea0183)
{
    nmea0183->sentence->checksum->value = (uint8_t)strtol(&nmea0183->sentence->buffer->data[nmea0183->sentence->buffer->sizeOf - 3], NULL, 16);
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

sensor_ID_t NMEA0183_GetSensorID(NMEA0183_t *nmea0183)
{
    return nmea0183->sentence->sensorID;
}

void NMEA0183_ParseData(NMEA0183_t *nmea0183)
{
    sensor_ID_t sensorType = NMEA0183_GetSensorID(nmea0183);

    for (uint8_t i = 0; i < nmea0183->registeredSensor->sizeOf; i++)
    {
        sensor_t *sensor = nmea0183->registeredSensor->sensor[i];

        if (sensor->sensorID == sensorType)
        {
            sensor->parserFunction(nmea0183->sentence->fields, sensor->data);
            if (sensor->data == NULL)
            {
                nmea0183->comunication->status = COMUNICATION_STATUS_ERROR;
            }

            return;
        }
    }

    nmea0183->comunication->status = COMUNICATION_STATUS_ERROR;
}

void *NMEA0183_GetDataAddress(NMEA0183_t *nmea0183)
{
    sensor_ID_t sensorType = NMEA0183_GetSensorID(nmea0183);

    for (uint8_t i = 0; i < nmea0183->registeredSensor->sizeOf; i++)
    {
        sensor_t *sensor = nmea0183->registeredSensor->sensor[i];

        if (sensor->sensorID == sensorType && sensor->data != NULL)
        {
            return sensor->data;
        }
    }

    nmea0183->comunication->status = COMUNICATION_STATUS_ERROR;

    return NULL;
}

void NMEA0183_PrintData(NMEA0183_t *nmea0183)
{

    sensor_ID_t sensorType = NMEA0183_GetSensorID(nmea0183);

    for (uint8_t i = 0; i < nmea0183->registeredSensor->sizeOf; i++)
    {
        sensor_t *sensor = nmea0183->registeredSensor->sensor[i];

        if (sensor->sensorID == sensorType)
        {
            sensor->printerFunction(sensor->data);
            return;
        }
    }

    nmea0183->comunication->status = COMUNICATION_STATUS_ERROR;
}

bool NMEA0183_IsDataReady(NMEA0183_t *nmea0183)
{
    return (nmea0183->comunication->stage == COMUNICATION_STAGE_DONE) &&
           (nmea0183->comunication->status == COMUNICATION_STATUS_OK);
}
