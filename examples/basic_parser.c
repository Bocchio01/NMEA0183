/**
 * @file basic_parser.c
 * @brief Basic NMEA0183 Parser Example
 *
 * Demonstrates how to use the NMEA0183 library with static allocation.
 * This example reads NMEA sentences and parses MWV and XDR data.
 */

#include <stdio.h>
#include <stdlib.h>

#include "nmea0183/nmea0183.h"
#include "nmea0183/sensors/mwv.h"
#include "nmea0183/sensors/xdr.h"

int main(void)
{
    /* Allocate all structures statically */
    NMEA0183_t nmea0183;

    /* Allocate sensor structures */
    sensor_t mwv_sensor;
    MWV_data_t mwv_data;

    sensor_t xdr_sensor;
    XDR_data_t xdr_data;

    /* Initialize NMEA0183 parser */
    NMEA0183_Init(&nmea0183);

    /* Initialize and register sensors */
    MWV_Init(&mwv_sensor, &mwv_data);
    XDR_Init(&xdr_sensor, &xdr_data);

    if (!NMEA0183_RegisterSensor(&nmea0183, &mwv_sensor))
    {
        fprintf(stderr, "Failed to register MWV sensor\n");
        return EXIT_FAILURE;
    }

    if (!NMEA0183_RegisterSensor(&nmea0183, &xdr_sensor))
    {
        fprintf(stderr, "Failed to register XDR sensor\n");
        return EXIT_FAILURE;
    }

    /* Open test data file */
    FILE *file = fopen("../examples/data/2023-12-21.txt", "r");
    if (file == NULL)
    {
        perror("Error opening file");
        return EXIT_FAILURE;
    }

    printf("NMEA0183 Parser - Static Allocation Example\n");
    printf("============================================\n\n");

    /* Process file character by character */
    int c;
    while ((c = fgetc(file)) != EOF)
    {
        /* Feed character to parser */
        NMEA0183_CharacterHandler(&nmea0183, (uint8_t)c);

        /* Check if complete sentence received */
        if (NMEA0183_IsDataReady(&nmea0183))
        {
            /* Print parsed data */
            NMEA0183_PrintData(&nmea0183);

            /* Alternative: Access data directly */
            sensor_ID_t sensorID = NMEA0183_GetSensorID(&nmea0183);
            void *dataPtr = NMEA0183_GetDataAddress(&nmea0183);

            if (sensorID == MWV && dataPtr != NULL)
            {
                MWV_data_t *mwv = (MWV_data_t *)dataPtr;
                printf("Direct access - Wind angle: %.2f, speed: %.2f\n\n", mwv->angle, mwv->speed);
            }
            else if (sensorID == XDR && dataPtr != NULL)
            {
                XDR_data_t *xdr = (XDR_data_t *)dataPtr;
                printf("Direct access - Temperature: %.2f %c\n\n", xdr->temperature, (char)xdr->unit);
            }

            /* Reset for next sentence */
            NMEA0183_Reset(&nmea0183);
        }
    }

    fclose(file);

    printf("\nParsing complete!\n");

    return EXIT_SUCCESS;
}