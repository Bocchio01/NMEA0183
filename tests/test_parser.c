/**
 * @file test_parser.c
 * @brief Unit tests for NMEA0183 sentence parser
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "nmea0183/nmea0183.h"
#include "nmea0183/sensors/mwv.h"
#include "nmea0183/sensors/xdr.h"

typedef struct
{
    const char *sentence;
    bool should_pass;
    sensor_ID_t expected_sensor;
    const char *description;
} test_case_t;

void test_sentence(NMEA0183_t *nmea, const char *sentence, bool should_pass,
                   sensor_ID_t expected_sensor, const char *description)
{
    printf("Test: %s\n", description);
    printf("  Sentence: %s\n", sentence);

    // Reset parser
    NMEA0183_Reset(nmea);

    // Feed sentence character by character
    for (size_t i = 0; i < strlen(sentence); i++)
    {
        NMEA0183_CharacterHandler(nmea, (uint8_t)sentence[i]);
    }

    bool is_ready = NMEA0183_IsDataReady(nmea);
    sensor_ID_t sensor_id = NMEA0183_GetSensorID(nmea);

    printf("  Expected: %s, sensor=%d\n", should_pass ? "PASS" : "FAIL", expected_sensor);
    printf("  Got: %s, sensor=%d\n", is_ready ? "PASS" : "FAIL", sensor_id);

    if (is_ready == should_pass &&
        (!should_pass || sensor_id == expected_sensor))
    {
        printf("  Result: PASS ✓\n\n");
    }
    else
    {
        printf("  Result: FAIL ✗\n\n");
        exit(EXIT_FAILURE);
    }
}

int main(void)
{
    printf("Testing NMEA0183 Parser\n");
    printf("=======================\n\n");

    // Initialize parser
    NMEA0183_t nmea;
    sensor_t mwv_sensor;
    MWV_data_t mwv_data;
    sensor_t xdr_sensor;
    XDR_data_t xdr_data;

    NMEA0183_Init(&nmea);
    MWV_Init(&mwv_sensor, &mwv_data);
    XDR_Init(&xdr_sensor, &xdr_data);

    NMEA0183_RegisterSensor(&nmea, &mwv_sensor);
    NMEA0183_RegisterSensor(&nmea, &xdr_sensor);

    // Valid sentences
    test_sentence(&nmea, "$IIMWV,123.4,R,5.6,N,A*3A\r\n", true, MWV, "Valid MWV sentence with CR+LF");
    test_sentence(&nmea, "$WIXDR,C,25.5,C,TempSensor*56\r\n", true, XDR, "Valid XDR sentence");
    test_sentence(&nmea, "$IIMWV,000.0,R,0.0,N,A*3D\r\n", true, MWV, "MWV with zero values");

    // Invalid sentences
    test_sentence(&nmea, "IIMWV,123.4,R,5.6,N,A*3A\r\n", false, UNKNOWN, "Missing start character");
    test_sentence(&nmea, "$IIMWV,123.4,R,5.6,N,A3A\r\n", false, UNKNOWN, "Missing checksum delimiter");
    test_sentence(&nmea, "$IIMWV,123.4,R,5.6,N,A*FF\r\n", false, UNKNOWN, "Invalid checksum");
    test_sentence(&nmea, "$XXXXX,123.4,R,5.6,N,A*3A\r\n", false, UNKNOWN, "Unknown sensor type");
    test_sentence(&nmea, "$IIMWV,123.4,R,5.6,N,A*\r\n", false, UNKNOWN, "Truncated sentence");

    // Edge cases
    test_sentence(&nmea, "$IIMWV,359.9,T,99.9,K,A*01\r\n", true, MWV, "Max valid values");
    test_sentence(&nmea, "$IIMWV,180.0,R,50.0,M,V*15\r\n", true, MWV, "Invalid status flag (still parseable)");

    printf("=======================\n");
    printf("All parser tests passed!\n");
    printf("=======================\n");

    return EXIT_SUCCESS;
}