/**
 * @file xdr.h
 * @brief XDR Sensor
 *
 * Sensor module: XDR (Transducer Measurement)
 *
 * @author Windex Team
 * @date 19/12/2023
 */

#ifndef XDR_H
#define XDR_H

#include "../sensors.h"

/* Transducer type enumeration */
typedef enum
{
    XDR_NAME_CV7 = 'C'
} XDR_type_t;

/* Temperature units enumeration */
typedef enum
{
    XDR_UNITS_CELSIUS = 'C',
    XDR_UNITS_FAHRENHEIT = 'F'
} XDR_units_t;

/* XDR sensor data structure */
typedef struct
{
    XDR_type_t type;
    float temperature;
    XDR_units_t unit;
    char name[32];
} XDR_data_t;

/**
 * @brief Initializes the XDR sensor structure.
 *
 * Caller must provide storage for both sensor_t and XDR_data_t.
 * This function only initializes the sensor_t structure with
 * appropriate function pointers.
 *
 * @param sensor Pointer to sensor_t structure to initialize
 * @param data Pointer to XDR_data_t structure for data storage
 */
void XDR_Init(sensor_t *sensor, XDR_data_t *data);

/**
 * @brief Resets the XDR sensor data to default values.
 *
 * @param data Pointer to XDR sensor data
 */
void XDR_Resetter(void *data);

/**
 * @brief Parses the XDR NMEA sentence.
 *
 * @param fields Pointer to fields of the XDR sentence
 * @param data Pointer to XDR sensor data
 *
 * @link https://gpsd.gitlab.io/gpsd/NMEA.html#_xdr_transducer_measurement
 * @note
 *         1 2   3 4            n
 *         | |   | |            |
 *  $--XDR,a,x.x,a,c--c, ..... *hh<CR><LF>
 *
 * Field Number:
 *    1. Transducer Type
 *    2. Measurement Data
 *    3. Units of measurement, C/F
 *    4. Name of transducer
 *    5. Checksum
 */
void XDR_Parser(const fields_t *fields, void *data);

/**
 * @brief Prints the XDR sensor data to stdout.
 *
 * @param data Pointer to XDR sensor data
 */
void XDR_Printer(const void *data);

#endif // XDR_H