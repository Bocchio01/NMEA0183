/**
 * @file sensors.c
 * @brief NMEA0183 Sensors Association
 *
 * @author Windex Team
 * @date 19/12/2023
 */

#include "sensors.h"

sensor_association_t sensorAssociation[] = {
    {"IIMWV", MWV},
    {"WIXDR", XDR},
    {"", UNKNOWN}};
