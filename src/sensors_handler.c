#include <stdio.h>
#include <stdint.h>

#include "sensors_handler.h"

sensor_association_t sensorAssociation[] = {
    {"IIMWV", MWV},
    {"WIXDR", XDR},
    {"", UNKNOWN}};
