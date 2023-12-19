
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "mwv.h"

registered_sensor_t MWV_Init()
{
    registered_sensor_t sensor = {
        .ID = MWV,
        .parserFunction = MWV_Parser,
        .printerFunction = MWV_Printer,
        .resetterFunction = MWV_Resetter,
        .data = (MWV_data_t *)malloc(sizeof(MWV_data_t)),
    };

    if (sensor.data == NULL)
    {
        fprintf(stderr, "Memory allocation failed for sensor MWV.\n");
        return sensor;
    }

    MWV_Resetter(sensor.data);

    return sensor;
}

void MWV_Resetter(void *data)
{
    MWV_data_t *typedData = (MWV_data_t *)data;

    typedData->angle = 0.0;
    typedData->reference = 'N';
    typedData->speed = 0.0;
    typedData->unit = MWV_UNITS_KNOTS;
    typedData->status = MWV_STATUS_VALID;
}

void MWV_Parser(char **fields, void *data)
{

    MWV_data_t *typedData = (MWV_data_t *)data;

    typedData->angle = strtof(fields[1], NULL);
    typedData->reference = (MWV_reference_t) * (fields[2]);
    typedData->speed = strtof(fields[3], NULL);
    typedData->unit = (MWV_units_t)*fields[4];
    typedData->status = (MWV_status_t)*fields[5];
}

void MWV_Printer(void *data)
{
    MWV_data_t *typedData = (MWV_data_t *)data;

    char referenceString[25];
    char unitString[25];
    char statusString[25];

    switch (typedData->reference)
    {
    case MWV_REFERENCE_RELATIVE:
        strcpy(referenceString, "Relative");
        break;
    case MWV_REFERENCE_TRUE:
        strcpy(referenceString, "True");
        break;
    default:
        strcpy(referenceString, "Unknown");
        break;
    }

    switch (typedData->unit)
    {
    case MWV_UNITS_KNOTS:
        strcpy(unitString, "Knots");
        break;
    case MWV_UNITS_METERS_PER_SECOND:
        strcpy(unitString, "Meters per second");
        break;
    case MWV_UNITS_KILOMETERS_PER_SECOND:
        strcpy(unitString, "Kilometers per second");
        break;
    default:
        strcpy(unitString, "Unknown");
        break;
    }

    switch (typedData->status)
    {
    case MWV_STATUS_VALID:
        strcpy(statusString, "Valid");
        break;
    case MWV_STATUS_INVALID:
        strcpy(statusString, "Invalid");
        break;
    default:
        strcpy(statusString, "Unknown");
        break;
    }

    time_t currentTime;
    time(&currentTime);
    struct tm *localTime = localtime(&currentTime);

    printf("----------------------\n");
    printf("MWV ([%02d:%02d:%02d])\n", localTime->tm_hour, localTime->tm_min, localTime->tm_sec);
    printf("Angle:\t%.2f\n", typedData->angle);
    printf("Ref.:\t%s\n", referenceString);
    printf("Speed:\t%.2f\n", typedData->speed);
    printf("Unit:\t%s\n", unitString);
    printf("Status:\t%s\n", statusString);
    printf("----------------------\n");
    printf("\n");
}