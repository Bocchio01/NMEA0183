/**
 * @file sensors.h
 * @brief NMEA0183 Sensors
 *
 * This file contains the definition of the structures that identify the
 * sensors and the functions that handle them.
 *
 * @author Windex Team
 * @date 19/12/2023
 */

#ifndef SENSORS_H
#define SENSORS_H

#include <stdint.h>

/* Configuration constants */
#define FIELDS_MAX_COUNT 20
#define FIELD_MAX_LENGTH 16
#define SENSOR_MAX_NUMBER 3

/* Parsed fields from NMEA sentence */
typedef struct
{
    uint8_t sizeOf;
    char data[FIELDS_MAX_COUNT][FIELD_MAX_LENGTH];
} fields_t;

/* Function pointer types for sensor operations */
typedef void (*parser_function_t)(const fields_t *fields, void *data);
typedef void (*printer_function_t)(const void *data);
typedef void (*reset_to_default_function_t)(void *data);

/* Sensor type enumeration */
typedef enum
{
    UNKNOWN = -1,
    MWV, /* Wind Speed and Angle */
    XDR, /* Transducer Measurement */
} sensor_ID_t;

/* Association between string identifier and sensor type */
typedef struct
{
    const char sensorTypeString[FIELD_MAX_LENGTH];
    sensor_ID_t sensorType;
} sensor_association_t;

/* Sensor descriptor structure */
typedef struct
{
    sensor_ID_t sensorID;
    parser_function_t parserFunction;
    printer_function_t printerFunction;
    reset_to_default_function_t resetterFunction;
    void *data;
} sensor_t;

/* Number of registered sensor types */
#define SENSOR_ASSOCIATION_COUNT 3

/* Global sensor association table */
extern const sensor_association_t sensorAssociation[SENSOR_ASSOCIATION_COUNT];

#endif // SENSORS_H