/**
 * @author Windex Team
 * @date 18/12/2023
 */

#ifndef MWV_H
#define MWV_H

#include "../sensors_handler.h"

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

registered_sensor_t MWV_Init();
void MWV_Resetter(void *data);

/**
 *        1   2 3   4 5
 *        |   | |   | |
 * $--MWV,x.x,a,x.x,a*hh<CR><LF>
 *
 * Field Number:
 *    1. Wind Angle, 0 to 359 degrees
 *    2. Reference, R = Relative, T = True
 *    3. Wind Speed
 *    4. Wind Speed Units, K/M/N
 *    5. Status, A = Data Valid, V = Invalid
 *    6. Checksum
 *
 * @link https://gpsd.gitlab.io/gpsd/NMEA.html#_mwv_wind_speed_and_angle
 */
void MWV_Parser(char **fields, void *data);
void MWV_Printer(void *data);

#endif // MWV_H
