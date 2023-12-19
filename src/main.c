#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "NMEA0183.h"
#include "sensors/mwv.h"

int main()
{

    int c;
    const char *filenameDatalogs = "../datalogs/2023-12-15.txt";

    FILE *file = fopen(filenameDatalogs, "r");

    if (file == NULL)
    {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }

    NMEA0183_t *nmea0183 = NMEA0183_Init();
    NMEA0183_RegisterSensor(nmea0183, MWV_Init());

    while ((c = fgetc(file)) != EOF)
    {
        NMEA0183_Update(nmea0183, c);
    }

    fclose(file);

    return 0;
}