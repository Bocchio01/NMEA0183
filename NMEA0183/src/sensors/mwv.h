/**
 * @file MWV.h
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

typedef enum
{
    MWV_REFERENCE_RELATIVE = 'R',
    MWV_REFERENCE_TRUE = 'T',
} MWV_reference_t;

typedef enum
{
    MWV_UNITS_KNOTS = 'N',
    MWV_UNITS_METERS_PER_SECOND = 'M',
    MWV_UNITS_KILOMETERS_PER_SECOND = 'K'
} MWV_units_t;

typedef enum
{
    MWV_STATUS_VALID = 'A',
    MWV_STATUS_INVALID = 'V'
} MWV_status_t;

typedef struct
{
    float angle;
    MWV_reference_t reference;
    float speed;
    MWV_units_t unit;
    MWV_status_t status;
} MWV_data_t;

/**
 * @brief Initializes the MWV sensor.
 *
 * @return sensor_t The MWV sensor.
 */
sensor_t MWV_Init();

/**
 * @brief Resets the MWV sensor data.
 *
 * @param data The MWV sensor data.
 */
void MWV_Resetter(void *data);

/**
 * @brief Parses the MWV sentence.
 *
 * @param fields The fields of the MWV sentence.
 * @param data The MWV sensor data.
 *
 * @link https://gpsd.gitlab.io/gpsd/NMEA.html#_mwv_wind_speed_and_angle
 * @note
 *        1   2 3   4 5
 *        |   | |   | |
 * $--MWV,x.x,a,x.x,a*hh<CR><LF>
 *
 * Field Number:
 *    1. Wind Angle, 0°-359°
 *    2. Reference, R/T
 *    3. Wind Speed
 *    4. Wind Speed Units, K/M/N
 *    5. Status, A/V
 *    6. Checksum
 */
void MWV_Parser(fields_t *fields, void *data);

/**
 * @brief Prints the MWV sensor data.
 *
 * @param data The MWV sensor data.
 */
void MWV_Printer(void *data);

#endif // MWV_H
