/**
 * @author Windex Team
 * @date 18/12/2023
 */

#ifndef XDR_H
#define XDR_H

#include "../sensors_handler.h"

typedef enum
{
    XDR_NAME_CV7 = 'C'
} XDR_type_t;

typedef enum
{
    XDR_UNITS_CELSIUS = 'C',
    XDR_UNITS_FAHRENHEIT = 'F'
} XDR_units_t;

typedef struct
{
    XDR_type_t type;
    float temperature;
    XDR_units_t unit;
    char name[];
} XDR_data_t;

sensor_t XDR_Init();
void XDR_Resetter(void *data);

/**
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
 *
 * See also: https://gpsd.gitlab.io/gpsd/NMEA.html#_xdr_transducer_measurement
 *
 */
void XDR_Parser(fields_t *fields, void *data);

void XDR_Printer(void *data);

#endif // XDR_H