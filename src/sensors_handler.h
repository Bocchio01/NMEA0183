/**
 * @file sensors_handler.h
 * @brief This file contains the definition of the data structures and
 * functions related to the sensors handler.
 *
 * The sensors handler is responsible for registering the sensors and
 * calling the correct parser function for each sensor.
 *
 * The sensors handler is implemented in Windex/src/sensors_handler.c.
 *
 * @see Windex/src/sensors_handler.c
 * @see Windex/src/NMEA0183.c
 *
 * @author Windex Team
 * @date 18/12/2023
 */

#ifndef SENSORS_HANDLER_H
#define SENSORS_HANDLER_H

#include <stdint.h>

#define FIELDS_MAX_COUNT 20
#define FIELD_MAX_LENGTH 10
#define MAX_SENSORS 10

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

sensor_association_t sensorAssociation[3];

#endif // SENSORS_HANDLER_H