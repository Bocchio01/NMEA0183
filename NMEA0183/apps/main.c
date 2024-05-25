#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "libs/cLOG/cLOG.h"

#include "src/NMEA0183.h"
#include "src/sensors/mwv.h"
#include "src/sensors/xdr.h"

int main()
{
    log_set_level(LOG_INFO);

    int c;

    FILE *file = fopen("../Matlab/Testing/Datasets/2023-12-21.txt", "r");
    if (file == NULL)
    {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }

    NMEA0183_t *nmea0183 = NMEA0183_Init();
    NMEA0183_RegisterSensor(nmea0183, MWV_Init());
    NMEA0183_RegisterSensor(nmea0183, XDR_Init());

    while ((c = fgetc(file)) != EOF)
    {
        NMEA0183_CharacterHandler(nmea0183, c);

        if (NMEA0183_IsDataReady(nmea0183))
        {
            NMEA0183_PrintData(nmea0183);
            NMEA0183_Reset(nmea0183);
        }
    }

    fclose(file);

    return EXIT_SUCCESS;
}