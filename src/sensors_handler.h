/**
 * @file sensors_handler.h
 * @brief NMEA0183 Sensors Handler
 *
 * This file contains the definition of the structures that identify the
 * sensors and the functions that handle them.
 *
 * @author Windex Team
 * @date 19/12/2023
 */

#ifndef SENSORS_HANDLER_H
#define SENSORS_HANDLER_H

#include <stdint.h>

#define FIELDS_MAX_COUNT 20
#define FIELD_MAX_LENGTH 10
#define SENSOR_MAX_NUMBER 3

typedef struct
{
    uint8_t sizeOf;
    char data[FIELDS_MAX_COUNT][FIELD_MAX_LENGTH];
} fields_t;

typedef void (*parser_function_t)(fields_t *fields, void *data);
typedef void (*printer_function_t)(void *data);
typedef void (*reset_to_default_function_t)(void *data);

typedef enum
{
    UNKNOWN = -1,
    MWV,
    XDR,
} sensor_ID_t;

typedef struct
{
    char sensorTypeString[FIELD_MAX_LENGTH];
    sensor_ID_t sensorType;
} sensor_association_t;

typedef struct
{
    sensor_ID_t sensorID;
    parser_function_t parserFunction;
    printer_function_t printerFunction;
    reset_to_default_function_t resetterFunction;
    void *data;
} sensor_t;

sensor_association_t sensorAssociation[SENSOR_MAX_NUMBER];

#endif // SENSORS_HANDLER_H