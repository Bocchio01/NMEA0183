/**
 * @file test_checksum.c
 * @brief Unit tests for NMEA0183 checksum computation
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "nmea0183/nmea0183.h"

typedef struct
{
    const char *sentence;
    uint8_t expected_checksum;
    const char *description;
} test_case_t;

int main(void)
{
    printf("Testing NMEA0183 Checksum Computation\n");
    printf("======================================\n\n");

    test_case_t tests[] = {
        {"$IIMWV,123.4,R,5.6,N,A*3A\r\n", 0x3A, "Valid MWV sentence"},
        {"$WIXDR,C,25.5,C,TempSensor*56\r\n", 0x56, "Valid XDR sentence"},
        {"$IIMWV,000.0,R,0.0,N,A*3D\r\n", 0x3D, "MWV with zero values"},
        {"$IIMWV,359.9,T,99.9,K,A*01\r\n", 0x01, "MWV with max values"},
        {"$WIXDR,C,0.0,C,Temp*52\r\n", 0x52, "XDR with zero temperature"},
    };

    int num_tests = sizeof(tests) / sizeof(test_case_t);
    int passed = 0;
    int failed = 0;

    for (int i = 0; i < num_tests; i++)
    {
        printf("Test %d: %s\n", i + 1, tests[i].description);
        printf("  Sentence: %s", tests[i].sentence);

        uint8_t computed = NMEA0183_ComputeChecksum(
            tests[i].sentence,
            (uint8_t)strlen(tests[i].sentence));

        printf("  Expected: 0x%02X\n", tests[i].expected_checksum);
        printf("  Computed: 0x%02X\n", computed);

        if (computed == tests[i].expected_checksum)
        {
            printf("  Result: PASS ✓\n\n");
            passed++;
        }
        else
        {
            printf("  Result: FAIL ✗\n\n");
            failed++;
        }
    }

    printf("======================================\n");
    printf("Results: %d passed, %d failed (total: %d)\n",
           passed, failed, num_tests);
    printf("======================================\n");

    return (failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}