/**
 * @file nmea0183.c
 * @brief NMEA0183 Protocol Handling Library Implementation
 *
 * @author Windex Team
 * @date 19/12/2023
 */

#include "nmea0183/nmea0183.h"
#include <string.h>
#include <stdlib.h>

/* Internal helper functions */
static inline void NMEA0183_ResetSentence(sentence_t *sentence);
static inline void NMEA0183_ResetComunication(comunication_t *comunication);
static inline void NMEA0183_ResetRegisteredSensor(registered_sensor_t *registeredSensor);
static inline void NMEA0183_AddToBuffer(NMEA0183_t *nmea0183, uint8_t c);
static void NMEA0183_AnalyzeData(NMEA0183_t *nmea0183);
static void NMEA0183_AnalyzeSensorID(NMEA0183_t *nmea0183);
static void NMEA0183_AnalyzeFields(NMEA0183_t *nmea0183);
static void NMEA0183_AnalyzeChecksum(NMEA0183_t *nmea0183);
static void NMEA0183_ParseData(NMEA0183_t *nmea0183);

void NMEA0183_Init(NMEA0183_t *nmea0183)
{
    if (nmea0183 == NULL)
        return;

    NMEA0183_ResetSentence(&nmea0183->sentence);
    NMEA0183_ResetComunication(&nmea0183->comunication);
    nmea0183->registeredSensor.sizeOf = 0;

    /* Initialize sensor array to NULL */
    for (uint8_t i = 0; i < SENSOR_MAX_NUMBER; i++)
    {
        nmea0183->registeredSensor.sensor[i] = NULL;
    }
}

static inline void NMEA0183_ResetSentence(sentence_t *sentence)
{
    sentence->buffer.sizeOf = 0;
    sentence->buffer.data[0] = '\0'; /* Only clear first byte for speed */
    sentence->sensorID = UNKNOWN;
    sentence->fields.sizeOf = 0;
    sentence->checksum.value = 0;
    sentence->checksum.isValid = false;
}

static inline void NMEA0183_ResetComunication(comunication_t *comunication)
{
    comunication->stage = COMUNICATION_STAGE_WAITING;
    comunication->status = COMUNICATION_STATUS_OK;
}

static inline void NMEA0183_ResetRegisteredSensor(registered_sensor_t *registeredSensor)
{
    for (uint8_t i = 0; i < registeredSensor->sizeOf; i++)
    {
        sensor_t *sensor = registeredSensor->sensor[i];
        if (sensor != NULL && sensor->resetterFunction != NULL)
        {
            sensor->resetterFunction(sensor->data);
        }
    }
}

void NMEA0183_Reset(NMEA0183_t *nmea0183)
{
    if (nmea0183 == NULL)
        return;

    NMEA0183_ResetSentence(&nmea0183->sentence);
    NMEA0183_ResetComunication(&nmea0183->comunication);
    NMEA0183_ResetRegisteredSensor(&nmea0183->registeredSensor);
}

bool NMEA0183_RegisterSensor(NMEA0183_t *nmea0183, sensor_t *sensor)
{
    if (nmea0183 == NULL || sensor == NULL)
    {
        return false;
    }

    if (nmea0183->registeredSensor.sizeOf >= SENSOR_MAX_NUMBER)
    {
        return false; /* Sensor array is full */
    }

    nmea0183->registeredSensor.sensor[nmea0183->registeredSensor.sizeOf] = sensor;
    nmea0183->registeredSensor.sizeOf++;

    return true;
}

void NMEA0183_CharacterHandler(NMEA0183_t *nmea0183, uint8_t c)
{
    if (nmea0183 == NULL)
        return;

    switch (nmea0183->comunication.stage)
    {
    case COMUNICATION_STAGE_WAITING:
        if (c == SENTENCE_CHARACTER_START)
        {
            nmea0183->comunication.stage = COMUNICATION_STAGE_INCOMING;
            NMEA0183_AddToBuffer(nmea0183, c);
        }
        break;

    case COMUNICATION_STAGE_INCOMING:
        NMEA0183_AddToBuffer(nmea0183, c);

        /* Check for sentence termination */
        if (c == SENTENCE_CHARACTER_END ||
            c == SENTENCE_CHARACTER_END_CR ||
            c == SENTENCE_CHARACTER_END_LF)
        {
            nmea0183->comunication.stage = COMUNICATION_STAGE_ANALYZING;
            NMEA0183_AnalyzeData(nmea0183);

            if (nmea0183->comunication.status == COMUNICATION_STATUS_OK)
            {
                nmea0183->comunication.stage = COMUNICATION_STAGE_DONE;
            }
        }
        break;

    case COMUNICATION_STAGE_DONE:
    case COMUNICATION_STAGE_ANALYZING:
        /* Ignore characters until reset */
        break;

    default:
        NMEA0183_Reset(nmea0183);
        break;
    }

    /* Reset on error */
    if (nmea0183->comunication.status == COMUNICATION_STATUS_ERROR)
    {
        NMEA0183_Reset(nmea0183);
    }
}

static inline void NMEA0183_AddToBuffer(NMEA0183_t *nmea0183, uint8_t c)
{
    if (nmea0183->sentence.buffer.sizeOf >= SENTENCE_MAX_LENGTH - 1)
    {
        nmea0183->comunication.status = COMUNICATION_STATUS_ERROR;
        return;
    }

    nmea0183->sentence.buffer.data[nmea0183->sentence.buffer.sizeOf] = (char)c;
    nmea0183->sentence.buffer.sizeOf++;
}

static void NMEA0183_AnalyzeData(NMEA0183_t *nmea0183)
{
    const uint8_t length = nmea0183->sentence.buffer.sizeOf;
    const char *data = nmea0183->sentence.buffer.data;

    /* Validate sentence start */
    if (data[0] != SENTENCE_CHARACTER_START)
    {
        nmea0183->comunication.status = COMUNICATION_STATUS_ERROR;
        return;
    }

    /* Validate sentence end */
    if (length < 1 || (data[length - 1] != SENTENCE_CHARACTER_END &&
                       data[length - 1] != SENTENCE_CHARACTER_END_LF &&
                       data[length - 1] != SENTENCE_CHARACTER_END_CR))
    {
        nmea0183->comunication.status = COMUNICATION_STATUS_ERROR;
        return;
    }

    /* Validate checksum delimiter position (should be 4th char from end: *HH\r or *HH\0) */
    if (length < 4 || data[length - 4] != SENTENCE_CHARACTER_CHECKSUM_START)
    {
        nmea0183->comunication.status = COMUNICATION_STATUS_ERROR;
        return;
    }

    /* Analyze components */
    NMEA0183_AnalyzeSensorID(nmea0183);
    if (nmea0183->sentence.sensorID == UNKNOWN)
    {
        nmea0183->comunication.status = COMUNICATION_STATUS_ERROR;
        return;
    }

    NMEA0183_AnalyzeFields(nmea0183);
    NMEA0183_AnalyzeChecksum(nmea0183);

    /* Verify checksum */
    const uint8_t computed = NMEA0183_ComputeChecksum(data, length);
    nmea0183->sentence.checksum.isValid = (nmea0183->sentence.checksum.value == computed);

    if (!nmea0183->sentence.checksum.isValid)
    {
        nmea0183->comunication.status = COMUNICATION_STATUS_ERROR;
        return;
    }

    /* Parse sensor-specific data */
    NMEA0183_ParseData(nmea0183);
}

static void NMEA0183_AnalyzeSensorID(NMEA0183_t *nmea0183)
{
    char sensorID[FIELD_MAX_LENGTH];
    const char *buffer = nmea0183->sentence.buffer.data;
    const uint8_t bufferSize = nmea0183->sentence.buffer.sizeOf;

    /* Extract sensor ID (skip '$' at position 0) */
    uint8_t i = 1;
    uint8_t j = 0;

    while (i < bufferSize && buffer[i] != SENTENCE_CHARACTER_DELIMITER && j < (FIELD_MAX_LENGTH - 1))
    {
        sensorID[j] = buffer[i];
        i++;
        j++;
    }
    sensorID[j] = '\0';

    /* Match against registered sensor types */
    const uint8_t numSensors = sizeof(sensorAssociation) / sizeof(sensorAssociation[0]);

    for (i = 0; i < numSensors; i++)
    {
        if (strcmp(sensorID, sensorAssociation[i].sensorTypeString) == 0)
        {
            nmea0183->sentence.sensorID = sensorAssociation[i].sensorType;
            return;
        }
    }

    nmea0183->sentence.sensorID = UNKNOWN;
}

static void NMEA0183_AnalyzeFields(NMEA0183_t *nmea0183)
{
    const char *buffer = nmea0183->sentence.buffer.data;
    const uint8_t bufferSize = nmea0183->sentence.buffer.sizeOf;
    fields_t *fields = &nmea0183->sentence.fields;

    fields->sizeOf = 0;

    uint8_t i = 0;
    uint8_t fieldIdx = 0;
    uint8_t charIdx = 0;

    /* Parse fields separated by delimiters */
    while (i < bufferSize && fieldIdx < FIELDS_MAX_COUNT)
    {
        const char c = buffer[i];

        /* Stop at checksum or end markers */
        if (c == SENTENCE_CHARACTER_CHECKSUM_START ||
            c == SENTENCE_CHARACTER_END ||
            c == SENTENCE_CHARACTER_END_CR ||
            c == SENTENCE_CHARACTER_END_LF)
        {
            break;
        }

        if (c == SENTENCE_CHARACTER_DELIMITER)
        {
            /* Terminate current field */
            fields->data[fieldIdx][charIdx] = '\0';
            fieldIdx++;
            charIdx = 0;
        }
        else if (charIdx < (FIELD_MAX_LENGTH - 1))
        {
            /* Add character to current field */
            fields->data[fieldIdx][charIdx] = c;
            charIdx++;
        }

        i++;
    }

    /* Terminate last field if exists */
    if (charIdx > 0 && fieldIdx < FIELDS_MAX_COUNT)
    {
        fields->data[fieldIdx][charIdx] = '\0';
        fieldIdx++;
    }

    fields->sizeOf = fieldIdx;
}

static void NMEA0183_AnalyzeChecksum(NMEA0183_t *nmea0183)
{
    const char *buffer = nmea0183->sentence.buffer.data;
    const uint8_t length = nmea0183->sentence.buffer.sizeOf;

    /* Checksum is at position length-3 and length-2 (HH format) */
    /* Use strtol for hex conversion */
    char checksumStr[3];
    checksumStr[0] = buffer[length - 3];
    checksumStr[1] = buffer[length - 2];
    checksumStr[2] = '\0';

    nmea0183->sentence.checksum.value = (uint8_t)strtol(checksumStr, NULL, 16);
}

uint8_t NMEA0183_ComputeChecksum(const char *buffer, uint8_t length)
{
    uint8_t checksum = 0;

    /* XOR all characters between $ and * */
    uint8_t i = 1; /* Skip '$' */
    while (i < length && buffer[i] != SENTENCE_CHARACTER_CHECKSUM_START)
    {
        checksum ^= (uint8_t)buffer[i];
        i++;
    }

    return checksum;
}

sensor_ID_t NMEA0183_GetSensorID(const NMEA0183_t *nmea0183)
{
    if (nmea0183 == NULL)
        return UNKNOWN;
    return nmea0183->sentence.sensorID;
}

static void NMEA0183_ParseData(NMEA0183_t *nmea0183)
{
    const sensor_ID_t sensorType = nmea0183->sentence.sensorID;
    const registered_sensor_t *regSensors = &nmea0183->registeredSensor;

    for (uint8_t i = 0; i < regSensors->sizeOf; i++)
    {
        sensor_t *sensor = regSensors->sensor[i];

        if (sensor != NULL && sensor->sensorID == sensorType)
        {
            if (sensor->parserFunction != NULL && sensor->data != NULL)
            {
                sensor->parserFunction(&nmea0183->sentence.fields, sensor->data);
                return;
            }
        }
    }

    /* Sensor not found or invalid */
    nmea0183->comunication.status = COMUNICATION_STATUS_ERROR;
}

void *NMEA0183_GetDataAddress(const NMEA0183_t *nmea0183)
{
    if (nmea0183 == NULL)
        return NULL;

    const sensor_ID_t sensorType = nmea0183->sentence.sensorID;
    const registered_sensor_t *regSensors = &nmea0183->registeredSensor;

    for (uint8_t i = 0; i < regSensors->sizeOf; i++)
    {
        const sensor_t *sensor = regSensors->sensor[i];

        if (sensor != NULL && sensor->sensorID == sensorType)
        {
            return sensor->data;
        }
    }

    return NULL;
}

void NMEA0183_PrintData(const NMEA0183_t *nmea0183)
{
    if (nmea0183 == NULL)
        return;

    const sensor_ID_t sensorType = nmea0183->sentence.sensorID;
    const registered_sensor_t *regSensors = &nmea0183->registeredSensor;

    for (uint8_t i = 0; i < regSensors->sizeOf; i++)
    {
        const sensor_t *sensor = regSensors->sensor[i];

        if (sensor != NULL && sensor->sensorID == sensorType)
        {
            if (sensor->printerFunction != NULL)
            {
                sensor->printerFunction(sensor->data);
            }
            return;
        }
    }
}

bool NMEA0183_IsDataReady(const NMEA0183_t *nmea0183)
{
    if (nmea0183 == NULL)
        return false;

    return (nmea0183->comunication.stage == COMUNICATION_STAGE_DONE) &&
           (nmea0183->comunication.status == COMUNICATION_STATUS_OK);
}