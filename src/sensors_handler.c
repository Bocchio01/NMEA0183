/**
 * @file sensors_handler.c
 * @brief NMEA0183 Sensors Handler Implementation
 *
 * @author Windex Team
 * @date 19/12/2023
 */

#include "sensors_handler.h"

sensor_association_t sensorAssociation[] = {
    {"IIMWV", MWV},
    {"WIXDR", XDR},
    {"", UNKNOWN}};
