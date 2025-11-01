/**
 * @file mwv.h
 * @brief MWV Sensor
 *
 * Sensor module: MWV (Wind Speed and Angle)
 *
 * @author Windex Team
 * @date 19/12/2023
 */

#ifndef MWV_H
#define MWV_H

#include "../sensors.h"

/* Wind reference enumeration */
typedef enum
{
    MWV_REFERENCE_RELATIVE = 'R',
    MWV_REFERENCE_TRUE = 'T',
} MWV_reference_t;

/* Wind speed units enumeration */
typedef enum
{
    MWV_UNITS_KNOTS = 'N',
    MWV_UNITS_METERS_PER_SECOND = 'M',
    MWV_UNITS_KILOMETERS_PER_SECOND = 'K'
} MWV_units_t;

/* Data validity status */
typedef enum
{
    MWV_STATUS_VALID = 'A',
    MWV_STATUS_INVALID = 'V'
} MWV_status_t;

/* MWV sensor data structure */
typedef struct
{
    float angle;
    MWV_reference_t reference;
    float speed;
    MWV_units_t unit;
    MWV_status_t status;
} MWV_data_t;

/**
 * @brief Initializes the MWV sensor structure.
 *
 * Caller must provide storage for both sensor_t and MWV_data_t.
 * This function only initializes the sensor_t structure with
 * appropriate function pointers.
 *
 * @param sensor Pointer to sensor_t structure to initialize
 * @param data Pointer to MWV_data_t structure for data storage
 */
void MWV_Init(sensor_t *sensor, MWV_data_t *data);

/**
 * @brief Resets the MWV sensor data to default values.
 *
 * @param data Pointer to MWV sensor data
 */
void MWV_Resetter(void *data);

/**
 * @brief Parses the MWV NMEA sentence.
 *
 * @param fields Pointer to fields of the MWV sentence
 * @param data Pointer to MWV sensor data
 *
 * @link https://gpsd.gitlab.io/gpsd/NMEA.html#_mwv_wind_speed_and_angle
 * @note
 *        1   2 3   4 5
 *        |   | |   | |
 * $--MWV,x.x,a,x.x,a*hh<CR><LF>
 *
 * Field Number:
 *    1. Wind Angle, 0-359°
 *    2. Reference, R/T
 *    3. Wind Speed
 *    4. Wind Speed Units, K/M/N
 *    5. Status, A/V
 *    6. Checksum
 */
void MWV_Parser(const fields_t *fields, void *data);

/**
 * @brief Prints the MWV sensor data to stdout.
 *
 * @param data Pointer to MWV sensor data
 */
void MWV_Printer(const void *data);

#endif // MWV_H