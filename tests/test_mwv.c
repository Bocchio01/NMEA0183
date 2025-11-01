/**
 * @file test_mwv.c
 * @brief Unit tests for MWV sensor parsing
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "nmea0183/nmea0183.h"
#include "nmea0183/sensors/mwv.h"

#define FLOAT_TOLERANCE 0.01f

typedef struct
{
    const char *sentence;
    float expected_angle;
    MWV_reference_t expected_reference;
    float expected_speed;
    MWV_units_t expected_unit;
    MWV_status_t expected_status;
    const char *description;
} mwv_test_case_t;

bool float_equal(float a, float b)
{
    return fabs(a - b) < FLOAT_TOLERANCE;
}

void test_mwv_sentence(NMEA0183_t *nmea, const mwv_test_case_t *test)
{
    printf("Test: %s\n", test->description);
    printf("  Sentence: %s\n", test->sentence);

    // Reset parser
    NMEA0183_Reset(nmea);

    // Feed sentence
    for (size_t i = 0; i < strlen(test->sentence); i++)
    {
        NMEA0183_CharacterHandler(nmea, (uint8_t)test->sentence[i]);
    }

    if (!NMEA0183_IsDataReady(nmea))
    {
        printf("  Result: FAIL ✗ (sentence not parsed)\n\n");
        exit(EXIT_FAILURE);
    }

    // Get parsed data
    MWV_data_t *data = (MWV_data_t *)NMEA0183_GetDataAddress(nmea);
    if (data == NULL)
    {
        printf("  Result: FAIL ✗ (data not available)\n\n");
        exit(EXIT_FAILURE);
    }

    // Verify parsed values
    bool pass = true;

    if (!float_equal(data->angle, test->expected_angle))
    {
        printf("  FAIL: Angle mismatch (expected %.1f, got %.1f)\n",
               test->expected_angle, data->angle);
        pass = false;
    }

    if (data->reference != test->expected_reference)
    {
        printf("  FAIL: Reference mismatch (expected %c, got %c)\n",
               (char)test->expected_reference, (char)data->reference);
        pass = false;
    }

    if (!float_equal(data->speed, test->expected_speed))
    {
        printf("  FAIL: Speed mismatch (expected %.1f, got %.1f)\n",
               test->expected_speed, data->speed);
        pass = false;
    }

    if (data->unit != test->expected_unit)
    {
        printf("  FAIL: Unit mismatch (expected %c, got %c)\n",
               (char)test->expected_unit, (char)data->unit);
        pass = false;
    }

    if (data->status != test->expected_status)
    {
        printf("  FAIL: Status mismatch (expected %c, got %c)\n",
               (char)test->expected_status, (char)data->status);
        pass = false;
    }

    if (pass)
    {
        printf("  Result: PASS ✓\n");
        printf("    Angle: %.1f°, Ref: %c, Speed: %.1f, Unit: %c, Status: %c\n\n",
               data->angle, (char)data->reference, data->speed,
               (char)data->unit, (char)data->status);
    }
    else
    {
        printf("  Result: FAIL ✗\n\n");
        exit(EXIT_FAILURE);
    }
}

int main(void)
{
    printf("Testing MWV Sensor Parsing\n");
    printf("==========================\n\n");

    // Initialize parser
    NMEA0183_t nmea;
    sensor_t mwv_sensor;
    MWV_data_t mwv_data;

    NMEA0183_Init(&nmea);
    MWV_Init(&mwv_sensor, &mwv_data);
    NMEA0183_RegisterSensor(&nmea, &mwv_sensor);

    // Test cases
    mwv_test_case_t tests[] = {
        {"$IIMWV,123.4,R,5.6,N,A*3A\r\n", 123.4f, MWV_REFERENCE_RELATIVE, 5.6f, MWV_UNITS_KNOTS, MWV_STATUS_VALID, "Normal wind data in knots"},
        {"$IIMWV,000.0,R,0.0,N,A*3D\r\n", 0.0f, MWV_REFERENCE_RELATIVE, 0.0f, MWV_UNITS_KNOTS, MWV_STATUS_VALID, "Zero wind (calm conditions)"},
        {"$IIMWV,359.9,T,99.9,K,A*01\r\n", 359.9f, MWV_REFERENCE_TRUE, 99.9f, MWV_UNITS_KILOMETERS_PER_SECOND, MWV_STATUS_VALID, "Max values with true reference"},
        {"$IIMWV,180.0,R,25.5,M,A*05\r\n", 180.0f, MWV_REFERENCE_RELATIVE, 25.5f, MWV_UNITS_METERS_PER_SECOND, MWV_STATUS_VALID, "Wind in meters per second"},
        {"$IIMWV,045.0,T,10.0,N,V*1C\r\n", 45.0f, MWV_REFERENCE_TRUE, 10.0f, MWV_UNITS_KNOTS, MWV_STATUS_INVALID, "Invalid status flag"},
        {"$IIMWV,270.5,R,15.2,N,A*0B\r\n", 270.5f, MWV_REFERENCE_RELATIVE, 15.2f, MWV_UNITS_KNOTS, MWV_STATUS_VALID, "Fractional angle value"},
    };

    int num_tests = sizeof(tests) / sizeof(mwv_test_case_t);

    for (int i = 0; i < num_tests; i++)
    {
        test_mwv_sentence(&nmea, &tests[i]);
    }

    printf("==========================\n");
    printf("All MWV tests passed (%d/%d)!\n", num_tests, num_tests);
    printf("==========================\n");

    return EXIT_SUCCESS;
}