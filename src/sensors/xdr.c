/**
 * @file XDR.c
 * @brief XDR Sensor Implementation
 *
 * @author Windex Team
 * @date 19/12/2023
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef STM32
#include "sensors/xdr.h"
#else
#include "xdr.h"
#endif

sensor_t XDR_Init()
{
    sensor_t sensor = {
        .sensorID = XDR,
        .parserFunction = XDR_Parser,
        .printerFunction = XDR_Printer,
        .resetterFunction = XDR_Resetter,
        .data = (XDR_data_t *)malloc(sizeof(XDR_data_t)),
    };

    if (sensor.data == NULL)
    {
        fprintf(stderr, "Memory allocation failed for sensor XDR.\n");
        return sensor;
    }

    XDR_Resetter(sensor.data);

    return sensor;
}

void XDR_Resetter(void *data)
{
    XDR_data_t *typedData = (XDR_data_t *)data;

    typedData->type = XDR_NAME_CV7;
    typedData->temperature = 0.0;
    typedData->unit = XDR_UNITS_CELSIUS;
    strcpy(typedData->name, "");
}

void XDR_Parser(fields_t *fields, void *data)
{
    XDR_data_t *typedData = (XDR_data_t *)data;

    typedData->type = (XDR_type_t)*fields->data[1];
    typedData->temperature = strtof(fields->data[2], NULL);
    typedData->unit = (XDR_units_t)*fields->data[3];
    strcpy(typedData->name, fields->data[4]);
}

void XDR_Printer(void *data)
{
    XDR_data_t *typedData = (XDR_data_t *)data;

    char unitString[25];

    switch (typedData->unit)
    {
    case XDR_UNITS_CELSIUS:
        strcpy(unitString, "Celsius");
        break;
    case XDR_UNITS_FAHRENHEIT:
        strcpy(unitString, "Fahrenheit");
        break;
    default:
        strcpy(unitString, "Unknown");
        break;
    }

    printf("----------XDR----------\n");
    printf("Type:\t%c\n", typedData->type);
    printf("Temp.:\t%.2f\n", typedData->temperature);
    printf("Unit:\t%s\n", unitString);
    printf("Name:\t%s\n", typedData->name);
    printf("-----------------------\n");
    printf("\n");
}