#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "src/NMEA0183.h"
#include "src/sensors/mwv.h"
#include "src/sensors/xdr.h"

#define NUMBER_OF_DATASETS 2
#define NUMBER_OF_BENCHMARKS_PER_DATASET 5

typedef enum
{
    PRINTF_DEBUG_STANDARD,
    PRINTF_DEBUG_NONE,
} test_t;

typedef struct
{
    char *name;
    test_t test;
    float *benchTimes;
    float benchTimeAverage;
} benchmark_t;

float average(float *array, int size);
float *runBenchmarks(test_t testCase, char datasetNames[][50], NMEA0183_t *nmea0183);
void printBenchInfo(FILE *outputFile, char *benchType, float benchTime);
void printBenchResult(FILE *outputFile, int n_test, int i_file, int j_bench_run, int counter, float time);
float computeBenchTime(clock_t tic, clock_t toc);

int main()
{

    time_t t = time(NULL);
    struct tm tm = *localtime(&t);

    char filename[21];
    snprintf(filename, sizeof(filename), "apps/bench_results/%04d-%02d-%02d.txt", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday);

    FILE *outputFile = fopen(filename, "w");
    if (outputFile == NULL)
    {
        perror("Error opening output file");
        return 1;
    }

    char datasetNames[NUMBER_OF_DATASETS][50] = {
        "../Matlab/Telemetry/Logs/2023-12-15.txt",
        "../Matlab/Testing/Datasets/2023-12-21.txt",
    };

    benchmark_t benchmarks[2] = {
        {"Printf Debug Standard", PRINTF_DEBUG_STANDARD, malloc(NUMBER_OF_DATASETS * sizeof(float)), 0},
        {"Printf Debug None", PRINTF_DEBUG_NONE, malloc(NUMBER_OF_DATASETS * sizeof(float)), 0},
    };

    NMEA0183_t *nmea0183 = NMEA0183_Init();
    NMEA0183_RegisterSensor(nmea0183, MWV_Init());
    NMEA0183_RegisterSensor(nmea0183, XDR_Init());

    for (uint8_t i = 0; i < sizeof(benchmarks) / sizeof(benchmark_t); i++)
    {
        benchmarks[i].benchTimes = runBenchmarks(benchmarks[i].test, datasetNames, nmea0183);
        benchmarks[i].benchTimeAverage = average(benchmarks[i].benchTimes, NUMBER_OF_DATASETS);

        printBenchInfo(outputFile, benchmarks[i].name, benchmarks[i].benchTimeAverage);
    }

    fclose(outputFile);

    return EXIT_SUCCESS;
}

float *runBenchmarks(test_t testCase, char datasetNames[][50], NMEA0183_t *nmea0183)
{
    float *benchTimes = malloc(sizeof(float) * NUMBER_OF_DATASETS);

    for (uint8_t i = 0; i < NUMBER_OF_DATASETS; i++)
    {
        FILE *file = fopen(datasetNames[i], "r");
        if (file == NULL)
        {
            perror("Error opening file");
            continue;
        }

        float benchTestTime[NUMBER_OF_BENCHMARKS_PER_DATASET] = {0};
        for (uint8_t j = 0; j < NUMBER_OF_BENCHMARKS_PER_DATASET; j++)
        {
            int c;
            int counter = 0;
            clock_t tic = clock();

            fseek(file, 0, SEEK_SET);
            while ((c = fgetc(file)) != EOF)
            {
                NMEA0183_CharacterHandler(nmea0183, c);

                switch (testCase)
                {
                case PRINTF_DEBUG_STANDARD:
                    if (NMEA0183_IsDataReady(nmea0183))
                    {
                        NMEA0183_PrintData(nmea0183);
                        NMEA0183_Reset(nmea0183);
                        counter++;
                    }
                    break;

                case PRINTF_DEBUG_NONE:
                    if (NMEA0183_IsDataReady(nmea0183))
                    {
                        NMEA0183_Reset(nmea0183);
                        counter++;
                    }
                    break;

                default:
                    break;
                }
            }

            clock_t toc = clock();
            benchTestTime[j] = computeBenchTime(tic, toc);
        }

        fclose(file);
        benchTimes[i] = average(benchTestTime, NUMBER_OF_BENCHMARKS_PER_DATASET);
    }

    return &benchTimes[0];
}

void printBenchInfo(FILE *outputFile, char *benchType, float benchTime)
{
    fprintf(outputFile, "#######################################################\n");
    fprintf(outputFile, "Bench type: %s\n", benchType);
    fprintf(outputFile, "Bench time: %f\n", benchTime);
    fprintf(outputFile, "#######################################################\n");
}

float computeBenchTime(clock_t tic, clock_t toc)
{
    return (double)(toc - tic) / CLOCKS_PER_SEC;
}

void printBenchResult(FILE *outputFile, int n_test, int i_file, int j_bench_run, int counter, float time)
{
    fprintf(outputFile, "%d\t%d\t%d\t%d\t%f\n", n_test, i_file, j_bench_run, counter, time);
}

float average(float *array, int size)
{
    float sum = 0;
    for (int i = 0; i < size; i++)
    {
        sum += array[i];
    }

    return sum / size;
}