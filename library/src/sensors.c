/**
 * @file sensors.c
 * @brief NMEA0183 Sensors Association
 *
 * @author Windex Team
 * @date 19/12/2023
 */

#include "nmea0183/sensors.h"

/* Sensor type to string mapping table */
const sensor_association_t sensorAssociation[] = {
    {"IIMWV", MWV},
    {"WIXDR", XDR},
    {"", UNKNOWN}};