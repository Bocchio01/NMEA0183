#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "../NMEA0183.h"

float randomFloat(float min, float max)
{
    return ((float)rand() / RAND_MAX) * (max - min) + min;
}

int main(int argc, char *argv[])
{
    char line[SENTENCE_MAX_LEGTH];
    char filenameDataset[] = "../telemetry/Generated_Dataset.txt";

    int datasetSize = argc == 2 ? atoi(argv[1]) : 50;

    NMEA0183_t *nmea0183 = NMEA0183_Init();

    FILE *file = fopen(filenameDataset, "w");

    if (file == NULL)
    {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }

    srand(time(NULL));

    for (int i = 0; i < datasetSize; ++i)
    {
        float windAngle = randomFloat(0.0, 359.0);
        float windSpeed = randomFloat(0.0, 100.0);
        float temperature = randomFloat(-10.0, 30.0);

        sprintf(line, "$IIMWV,%.1f,R,%.1f,N,A", windAngle, windSpeed);
        fprintf(file, "$IIMWV,%.1f,R,%.1f,N,A*%02X\n", windAngle, windSpeed, NMEA0183_ComputeChecksum(line, strlen(line)));

        sprintf(line, "$WIXDR,C,%.1f,C,,", temperature);
        fprintf(file, "$WIXDR,C,%.1f,C,,*%02X\n", temperature, NMEA0183_ComputeChecksum(line, strlen(line)));

        fprintf(file, "$PLCJ,6AFE,6E00,AA,8373,5353\n");
        fprintf(file, "$PLCJE8BC8,69A8,2D00,2165,6F,\n");
    }

    fclose(file);

    return 0;
}
