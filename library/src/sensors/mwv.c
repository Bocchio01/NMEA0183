/**
 * @file mwv.c
 * @brief MWV Sensor Implementation
 *
 * @author Windex Team
 * @date 19/12/2023
 */

#include "nmea0183/sensors/mwv.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void MWV_Init(sensor_t *sensor, MWV_data_t *data)
{
    if (sensor == NULL || data == NULL)
        return;

    sensor->sensorID = MWV;
    sensor->parserFunction = MWV_Parser;
    sensor->printerFunction = MWV_Printer;
    sensor->resetterFunction = MWV_Resetter;
    sensor->data = data;

    MWV_Resetter(data);
}

void MWV_Resetter(void *data)
{
    if (data == NULL)
        return;

    MWV_data_t *typedData = (MWV_data_t *)data;

    typedData->angle = 0.0f;
    typedData->reference = MWV_REFERENCE_RELATIVE;
    typedData->speed = 0.0f;
    typedData->unit = MWV_UNITS_KNOTS;
    typedData->status = MWV_STATUS_VALID;
}

void MWV_Parser(const fields_t *fields, void *data)
{
    if (fields == NULL || data == NULL)
        return;

    MWV_data_t *typedData = (MWV_data_t *)data;

    /* Parse angle (field 1) */
    if (fields->sizeOf > 1)
    {
        typedData->angle = strtof(fields->data[1], NULL);
    }

    /* Parse reference (field 2) */
    if (fields->sizeOf > 2 && fields->data[2][0] != '\0')
    {
        typedData->reference = (MWV_reference_t)fields->data[2][0];
    }

    /* Parse speed (field 3) */
    if (fields->sizeOf > 3)
    {
        typedData->speed = strtof(fields->data[3], NULL);
    }

    /* Parse unit (field 4) */
    if (fields->sizeOf > 4 && fields->data[4][0] != '\0')
    {
        typedData->unit = (MWV_units_t)fields->data[4][0];
    }

    /* Parse status (field 5) */
    if (fields->sizeOf > 5 && fields->data[5][0] != '\0')
    {
        typedData->status = (MWV_status_t)fields->data[5][0];
    }
}

void MWV_Printer(const void *data)
{
    if (data == NULL)
        return;

    const MWV_data_t *typedData = (const MWV_data_t *)data;

    const char *referenceString;
    const char *unitString;
    const char *statusString;

    /* Convert reference to string */
    switch (typedData->reference)
    {
    case MWV_REFERENCE_RELATIVE:
        referenceString = "Relative";
        break;
    case MWV_REFERENCE_TRUE:
        referenceString = "True";
        break;
    default:
        referenceString = "Unknown";
        break;
    }

    /* Convert unit to string */
    switch (typedData->unit)
    {
    case MWV_UNITS_KNOTS:
        unitString = "Knots";
        break;
    case MWV_UNITS_METERS_PER_SECOND:
        unitString = "Meters per second";
        break;
    case MWV_UNITS_KILOMETERS_PER_SECOND:
        unitString = "Kilometers per second";
        break;
    default:
        unitString = "Unknown";
        break;
    }

    /* Convert status to string */
    switch (typedData->status)
    {
    case MWV_STATUS_VALID:
        statusString = "Valid";
        break;
    case MWV_STATUS_INVALID:
        statusString = "Invalid";
        break;
    default:
        statusString = "Unknown";
        break;
    }

    printf("MWV--------------------\n");
    printf("Angle:\t%.2f\n", typedData->angle);
    printf("Ref.:\t%s\n", referenceString);
    printf("Speed:\t%.2f\n", typedData->speed);
    printf("Unit:\t%s\n", unitString);
    printf("Status:\t%s\n", statusString);
    printf("-----------------------\n");
}