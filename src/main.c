#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#include "NMEA0183.h"
#include "sensors/mwv.h"
#include "sensors/xdr.h"

int main()
{

    clock_t tic = clock();

    int c;
    char filenameDataset[] = "../Matlab/Telemetry/Logger/2023-12-15.txt";

    FILE *file = fopen(filenameDataset, "r");

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
            // NMEA0183_PrintData(nmea0183);
            NMEA0183_Reset(nmea0183);
        }
    }

    fclose(file);

    clock_t toc = clock();
    printf("Elapsed: %f seconds\n", (double)(toc - tic) / CLOCKS_PER_SEC);

    return 0;
}