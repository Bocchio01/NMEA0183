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

typedef struct NMEA0183_t NMEA0183_t;
typedef void (*parser_function_t)(char **fields, void *data);
typedef void (*printer_function_t)(void *data);
typedef void (*reset_to_default_function_t)(void *data);

typedef enum
{
    UNKNOWN = -1,
    MWV,
    XDR,
} sensor_t;

typedef struct
{
    const char *sensorTypeString;
    sensor_t sensorType;
} sensor_association_t;

typedef struct
{
    sensor_t ID;
    parser_function_t parserFunction;
    printer_function_t printerFunction;
    reset_to_default_function_t resetterFunction;
    void *data;
} registered_sensor_t;

#endif // SENSORS_HANDLER_H