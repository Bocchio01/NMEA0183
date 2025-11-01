/**
 * @file nmea0183.h
 * @brief NMEA0183 Protocol Handling Library
 *
 * This library provides functionality for handling the NMEA0183 protocol,
 * a standard for communication between marine electronic devices.
 *
 * Features:
 * - Static memory allocation (no malloc/free)
 * - Optimized for embedded systems
 * - Character-by-character streaming parser
 * - Extensible sensor registration system
 *
 * @author Windex Team
 * @date 19/12/2023
 *
 * @see sensors.h
 */

#ifndef NMEA0183_H
#define NMEA0183_H

#include <stdbool.h>
#include <stdint.h>

#include "sensors.h"

/* Configuration constants */
#define SENTENCE_MAX_LENGTH 83                 /* 82 chars + null terminator */
#define SENTENCE_CHARACTER_START 0x24          /* '$' */
#define SENTENCE_CHARACTER_DELIMITER 0x2C      /* ',' */
#define SENTENCE_CHARACTER_END 0x00            /* NULL */
#define SENTENCE_CHARACTER_END_CR 0x0D         /* '\r' */
#define SENTENCE_CHARACTER_END_LF 0x0A         /* '\n' */
#define SENTENCE_CHARACTER_CHECKSUM_START 0x2A /* '*' */

/* Communication stages */
typedef enum
{
    COMUNICATION_STAGE_WAITING,
    COMUNICATION_STAGE_INCOMING,
    COMUNICATION_STAGE_ANALYZING,
    COMUNICATION_STAGE_DONE,
} comunication_stage_t;

/* Communication status */
typedef enum
{
    COMUNICATION_STATUS_OK,
    COMUNICATION_STATUS_ERROR
} comunication_status_t;

/* Sentence buffer */
typedef struct
{
    uint8_t sizeOf;
    char data[SENTENCE_MAX_LENGTH];
} buffer_t;

/* Checksum information */
typedef struct
{
    uint8_t value;
    bool isValid;
} checksum_t;

/* Parsed sentence structure */
typedef struct
{
    buffer_t buffer;
    sensor_ID_t sensorID;
    fields_t fields;
    checksum_t checksum;
} sentence_t;

/* Communication state */
typedef struct
{
    comunication_stage_t stage;
    comunication_status_t status;
} comunication_t;

/* Registered sensors array */
typedef struct
{
    uint8_t sizeOf;
    sensor_t *sensor[SENSOR_MAX_NUMBER];
} registered_sensor_t;

/**
 * The main structure representing the NMEA0183 protocol handling.
 * All memory is statically allocated for embedded systems.
 */
typedef struct
{
    sentence_t sentence;
    comunication_t comunication;
    registered_sensor_t registeredSensor;
} NMEA0183_t;

/**
 * @brief Initialize NMEA0183 struct
 *
 * Initialize NMEA0183 struct with default values.
 * No dynamic allocation - all memory must be provided by caller.
 *
 * @param nmea0183 Pointer to NMEA0183_t struct to initialize
 */
void NMEA0183_Init(NMEA0183_t *nmea0183);

/**
 * @brief Reset NMEA0183 struct
 *
 * Reset data of NMEA0183 struct to its default value.
 *
 * @param nmea0183 Pointer to NMEA0183_t struct to reset
 */
void NMEA0183_Reset(NMEA0183_t *nmea0183);

/**
 * @brief Register sensor
 *
 * Register a new sensor in NMEA0183 struct.
 * The sensor data structure must be statically allocated by caller.
 *
 * @param nmea0183 Pointer to NMEA0183_t struct
 * @param sensor Pointer to sensor to register (must remain valid)
 * @return true if registration successful, false if sensor array is full
 */
bool NMEA0183_RegisterSensor(NMEA0183_t *nmea0183, sensor_t *sensor);

/**
 * @brief Handle incoming character
 *
 * Handle incoming character from the NMEA0183 ecosystem.
 * This function must be called every time a new character is received.
 * Optimized for streaming data processing.
 *
 * @param nmea0183 Pointer to NMEA0183_t struct
 * @param c Incoming character
 *
 * @see NMEA0183_IsDataReady()
 * @see NMEA0183_Reset()
 */
void NMEA0183_CharacterHandler(NMEA0183_t *nmea0183, uint8_t c);

/**
 * @brief Get sensor ID
 *
 * Get sensor ID stored in the NMEA0183 struct.
 *
 * @param nmea0183 Pointer to NMEA0183_t struct
 * @return sensor_ID_t Sensor ID
 */
sensor_ID_t NMEA0183_GetSensorID(const NMEA0183_t *nmea0183);

/**
 * @brief Get data address
 *
 * Get the address of the data stored in the registered sensor
 * corresponding to the sensor that sent the last valid sentence.
 *
 * @param nmea0183 Pointer to NMEA0183_t struct
 * @return void* Address of the data, or NULL if not found
 */
void *NMEA0183_GetDataAddress(const NMEA0183_t *nmea0183);

/**
 * @brief Print data
 *
 * Print data by calling the print function defined in the sensor struct.
 * If the sensor is not registered, the data are not printed.
 *
 * @param nmea0183 Pointer to NMEA0183_t struct
 */
void NMEA0183_PrintData(const NMEA0183_t *nmea0183);

/**
 * @brief Check if data is ready
 *
 * Check if a complete valid sentence has been received and parsed.
 *
 * @param nmea0183 Pointer to NMEA0183_t struct
 * @return bool true if data is ready, false otherwise
 */
bool NMEA0183_IsDataReady(const NMEA0183_t *nmea0183);

/**
 * @brief Compute checksum
 *
 * Compute XOR checksum of NMEA sentence.
 * Exposed for testing purposes.
 *
 * @param buffer Pointer to buffer containing sentence
 * @param length Length of the sentence
 * @return uint8_t Computed checksum value
 */
uint8_t NMEA0183_ComputeChecksum(const char *buffer, uint8_t length);

#endif // NMEA0183_H