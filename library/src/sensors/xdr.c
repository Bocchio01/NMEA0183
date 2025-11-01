/**
 * @file xdr.c
 * @brief XDR Sensor Implementation
 *
 * @author Windex Team
 * @date 19/12/2023
 */

#include "nmea0183/sensors/xdr.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void XDR_Init(sensor_t *sensor, XDR_data_t *data)
{
    if (sensor == NULL || data == NULL)
        return;

    sensor->sensorID = XDR;
    sensor->parserFunction = XDR_Parser;
    sensor->printerFunction = XDR_Printer;
    sensor->resetterFunction = XDR_Resetter;
    sensor->data = data;

    XDR_Resetter(data);
}

void XDR_Resetter(void *data)
{
    if (data == NULL)
        return;

    XDR_data_t *typedData = (XDR_data_t *)data;

    typedData->type = XDR_NAME_CV7;
    typedData->temperature = 0.0f;
    typedData->unit = XDR_UNITS_CELSIUS;
    typedData->name[0] = '\0';
}

void XDR_Parser(const fields_t *fields, void *data)
{
    if (fields == NULL || data == NULL)
        return;

    XDR_data_t *typedData = (XDR_data_t *)data;

    /* Parse transducer type (field 1) */
    if (fields->sizeOf > 1 && fields->data[1][0] != '\0')
    {
        typedData->type = (XDR_type_t)fields->data[1][0];
    }

    /* Parse temperature value (field 2) */
    if (fields->sizeOf > 2)
    {
        typedData->temperature = strtof(fields->data[2], NULL);
    }

    /* Parse units (field 3) */
    if (fields->sizeOf > 3 && fields->data[3][0] != '\0')
    {
        typedData->unit = (XDR_units_t)fields->data[3][0];
    }

    /* Parse transducer name (field 4) */
    if (fields->sizeOf > 4)
    {
        strncpy(typedData->name, fields->data[4], sizeof(typedData->name) - 1);
        typedData->name[sizeof(typedData->name) - 1] = '\0'; /* Ensure null termination */
    }
}

void XDR_Printer(const void *data)
{
    if (data == NULL)
        return;

    const XDR_data_t *typedData = (const XDR_data_t *)data;

    const char *unitString;

    /* Convert unit to string */
    switch (typedData->unit)
    {
    case XDR_UNITS_CELSIUS:
        unitString = "Celsius";
        break;
    case XDR_UNITS_FAHRENHEIT:
        unitString = "Fahrenheit";
        break;
    default:
        unitString = "Unknown";
        break;
    }

    printf("XDR--------------------\n");
    printf("Type:\t%c\n", (char)typedData->type);
    printf("Temp.:\t%.2f\n", typedData->temperature);
    printf("Unit:\t%s\n", unitString);
    printf("Name:\t%s\n", typedData->name);
    printf("-----------------------\n");
}