/**
 * @file test_xdr.c
 * @brief Unit tests for XDR sensor parsing
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "nmea0183/nmea0183.h"
#include "nmea0183/sensors/xdr.h"

#define FLOAT_TOLERANCE 0.01f

typedef struct
{
    const char *sentence;
    XDR_type_t expected_type;
    float expected_temperature;
    XDR_units_t expected_unit;
    const char *expected_name;
    const char *description;
} xdr_test_case_t;

bool float_equal(float a, float b)
{
    return fabs(a - b) < FLOAT_TOLERANCE;
}

void test_xdr_sentence(NMEA0183_t *nmea, const xdr_test_case_t *test)
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
    XDR_data_t *data = (XDR_data_t *)NMEA0183_GetDataAddress(nmea);
    if (data == NULL)
    {
        printf("  Result: FAIL ✗ (data not available)\n\n");
        exit(EXIT_FAILURE);
    }

    // Verify parsed values
    bool pass = true;

    if (data->type != test->expected_type)
    {
        printf("  FAIL: Type mismatch (expected %c, got %c)\n", (char)test->expected_type, (char)data->type);
        pass = false;
    }

    if (!float_equal(data->temperature, test->expected_temperature))
    {
        printf("  FAIL: Temperature mismatch (expected %.1f, got %.1f)\n", test->expected_temperature, data->temperature);
        pass = false;
    }

    if (data->unit != test->expected_unit)
    {
        printf("  FAIL: Unit mismatch (expected %c, got %c)\n", (char)test->expected_unit, (char)data->unit);
        pass = false;
    }

    if (strcmp(data->name, test->expected_name) != 0)
    {
        printf("  FAIL: Name mismatch (expected '%s', got '%s')\n", test->expected_name, data->name);
        pass = false;
    }

    if (pass)
    {
        printf("  Result: PASS ✓\n");
        printf("    Type: %c, Temp: %.1f°%c, Name: %s\n\n", (char)data->type, data->temperature, (char)data->unit, data->name);
    }
    else
    {
        printf("  Result: FAIL ✗\n\n");
        exit(EXIT_FAILURE);
    }
}

int main(void)
{
    printf("Testing XDR Sensor Parsing\n");
    printf("==========================\n\n");

    // Initialize parser
    NMEA0183_t nmea;
    sensor_t xdr_sensor;
    XDR_data_t xdr_data;

    NMEA0183_Init(&nmea);
    XDR_Init(&xdr_sensor, &xdr_data);
    NMEA0183_RegisterSensor(&nmea, &xdr_sensor);

    // Test cases
    xdr_test_case_t tests[] = {
        {"$WIXDR,C,25.5,C,TempSensor*56\r\n", XDR_NAME_CV7, 25.5f, XDR_UNITS_CELSIUS, "TempSensor", "Normal temperature in Celsius"},
        {"$WIXDR,C,0.0,C,Temp*52\r\n", XDR_NAME_CV7, 0.0f, XDR_UNITS_CELSIUS, "Temp", "Zero temperature"},
        {"$WIXDR,C,-10.5,C,OutdoorT*6B\r\n", XDR_NAME_CV7, -10.5f, XDR_UNITS_CELSIUS, "OutdoorT", "Negative temperature"},
        {"$WIXDR,C,98.6,F,BodyTemp*50\r\n", XDR_NAME_CV7, 98.6f, XDR_UNITS_FAHRENHEIT, "BodyTemp", "Temperature in Fahrenheit"},
        {"$WIXDR,C,100.0,C,Boiling*37\r\n", XDR_NAME_CV7, 100.0f, XDR_UNITS_CELSIUS, "Boiling", "High temperature"},
        {"$WIXDR,C,23.456,C,Precise*13\r\n", XDR_NAME_CV7, 23.456f, XDR_UNITS_CELSIUS, "Precise", "High precision temperature"},
        {"$WIXDR,C,15.0,C,CV7*68\r\n", XDR_NAME_CV7, 15.0f, XDR_UNITS_CELSIUS, "CV7", "Short sensor name"},
    };

    int num_tests = sizeof(tests) / sizeof(xdr_test_case_t);

    for (int i = 0; i < num_tests; i++)
    {
        test_xdr_sentence(&nmea, &tests[i]);
    }

    printf("==========================\n");
    printf("All XDR tests passed (%d/%d)!\n", num_tests, num_tests);
    printf("==========================\n");

    return EXIT_SUCCESS;
}