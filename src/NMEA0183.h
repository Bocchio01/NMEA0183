/**
 * @file NMEA0183.h
 * @brief NMEA0183 Protocol Handling Library
 *
 * This library provides functionality for handling the NMEA0183 protocol,
 * a standard for communication between marine electronic devices.
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
#include <string.h>

#include "sensors.h"

#define SENTENCE_MAX_LEGTH 82 + 1
#define SENTENCE_CHARACTER_START 0x24
#define SENTENCE_CHARACTER_DELIMITER 0x2C
#define SENTENCE_CHARACTER_END 0x00
#define SENTENCE_CHARACTER_END_CR 0x0D
#define SENTENCE_CHARACTER_END_LF 0x0A
#define SENTENCE_CHARACTER_CHECKSUM_START 0x2A

typedef enum
{
	COMUNICATION_STAGE_WAITING,
	COMUNICATION_STAGE_INCOMING,
	COMUNICATION_STAGE_ANALYZING,
	COMUNICATION_STAGE_DONE,
} comunication_stage_t;

typedef enum
{
	COMUNICATION_STATUS_OK,
	COMUNICATION_STATUS_ERROR
} comunication_status_t;

typedef struct
{
	uint8_t sizeOf;
	char data[SENTENCE_MAX_LEGTH];
} buffer_t;

// TODO: resolve circular dependency
// struct fields_t: Declared in sensors.h;

typedef struct
{
	uint8_t value;
	bool isValid;
} checksum_t;

typedef struct
{
	buffer_t *buffer;
	sensor_ID_t sensorID;
	fields_t *fields;
	checksum_t *checksum;
} sentence_t;

typedef struct
{
	comunication_stage_t stage;
	comunication_status_t status;
} comunication_t;

typedef struct
{
	uint8_t sizeOf;
	sensor_t *sensor[SENSOR_MAX_NUMBER];
} registered_sensor_t;

/**
 * The main structure representing the NMEA0183 protocol handling.
 */
typedef struct
{
	sentence_t *sentence;
	comunication_t *comunication;
	registered_sensor_t *registeredSensor;
} NMEA0183_t;

/**
 * @brief Initialize NMEA0183_t struct
 *
 * Create space in memory for NMEA0183_t struct and initialize it with
 * default values.
 *
 * @return NMEA0183_t*
 *
 * @see NMEA0183_InitSentence()
 * @see NMEA0183_InitComunication()
 * @see NMEA0183_InitRegisteredSensor()
 */
NMEA0183_t *NMEA0183_Init();

/**
 * @brief Initialize sentence_t struct
 *
 * Create space in memory for sentence_t struct and initialize it with
 * default values.
 *
 * @return sentence_t*
 *
 * @see NMEA0183_ResetSentence()
 */
sentence_t *NMEA0183_InitSentence();

/**
 * @brief Initialize comunication_t struct
 *
 * Create space in memory for comunication_t struct and initialize it
 * with default values.
 *
 * @return comunication_t*
 *
 * @see NMEA0183_ResetComunication()
 */
comunication_t *NMEA0183_InitComunication();

/**
 * @brief Initialize registered_sensor_t struct
 *
 * Create space in memory for registered_sensor_t struct and reset data
 * of each already registered sensor to its default value.
 *
 * @return registered_sensor_t*
 *
 * @see NMEA0183_ResetRegisteredSensor()
 */
registered_sensor_t *NMEA0183_InitRegisteredSensor();

/**
 * @brief Reset NMEA0183_t struct
 *
 * Reset data of NMEA0183_t struct to its default value.
 *
 * @param *nmea0183 pointer to NMEA0183_t struct to reset
 */
void NMEA0183_Reset(NMEA0183_t *nmea0183);

/**
 * @brief Reset sentence_t struct
 *
 * Reset data of sentence_t struct to its default value.
 *
 * @param *sentence pointer to sentence_t struct to reset
 */
void NMEA0183_ResetSentence(sentence_t *sentence);

/**
 * @brief Reset comunication_t struct
 *
 * Reset data of comunication_t struct to its default value.
 *
 * @param *comunication pointer to comunication_t struct to reset
 */
void NMEA0183_ResetComunication(comunication_t *comunication);

/**
 * @brief Reset registered_sensor_t struct
 *
 * Reset data of registered_sensor_t struct to its default value.
 *
 * @param *registeredSensor pointer to registered_sensor_t struct to reset
 */
void NMEA0183_ResetRegisteredSensor(registered_sensor_t *registeredSensor);

/**
 * @brief Register sensor
 *
 * Register a new sensor in NMEA0183_t struct.
 *
 * @param *nmea0183 pointer to NMEA0183_t struct
 * @param sensor_value sensor to register
 */
void NMEA0183_RegisterSensor(NMEA0183_t *nmea0183, sensor_t sensor_value);

/**
 * @brief Handle incoming character
 *
 * Handle incoming character from the NMEA0183 ecosystem.
 * This function must be is called every time a new character is received.
 *
 * @param *nmea0183 pointer to NMEA0183_t struct
 * @param c incoming character
 *
 * @see NMEA0183_AddToBuffer()
 * @see NMEA0183_AnalyzeData()
 * @see NMEA0183_Reset()
 */
void NMEA0183_CharacterHandler(NMEA0183_t *nmea0183, uint8_t c);

/**
 * @brief Add character to buffer
 *
 * Add a new character to the buffer of the NMEA0183_t struct.
 * If the buffer is full, a reset action is required.
 *
 * @param *nmea0183 pointer to NMEA0183_t struct
 * @param c incoming character
 */
void NMEA0183_AddToBuffer(NMEA0183_t *nmea0183, uint8_t c);

/**
 * @brief Analyze data
 *
 * Analyze data of the NMEA0183_t struct.
 * This function must be called when an end of sentence is detected.
 *
 * @param *nmea0183 pointer to NMEA0183_t struct
 *
 * @see NMEA0183_GetSensorID()
 * @see NMEA0183_GetFields()
 * @see NMEA0183_GetChecksum()
 * @see NMEA0183_ComputeChecksum()
 * @see NMEA0183_ParseData()
 */
void NMEA0183_AnalyzeData(NMEA0183_t *nmea0183);

/**
 * @brief Get sensor ID
 *
 * Get sensor ID based on the first field of the sentence and store it
 * in the NMEA0183_t struct.
 *
 * @param *nmea0183 pointer to NMEA0183_t struct
 */
void NMEA0183_GetSensorID(NMEA0183_t *nmea0183);

/**
 * @brief Get fields
 *
 * Get fields of the sentence and store them in the NMEA0183_t struct.
 *
 * @param *nmea0183 pointer to NMEA0183_t struct
 */
void NMEA0183_GetFields(NMEA0183_t *nmea0183);

/**
 * @brief Get checksum
 *
 * Get checksum of the sentence and store it in the NMEA0183_t struct.
 *
 * @param *nmea0183 pointer to NMEA0183_t struct
 */
void NMEA0183_GetChecksum(NMEA0183_t *nmea0183);

/**
 * @brief Compute checksum
 *
 * Compute checksum of the sentence.
 *
 * @param *buffer pointer to buffer of the sentence
 * @param length length of the sentence
 *
 * @return uint8_t checksum
 */
uint8_t NMEA0183_ComputeChecksum(char *buffer, uint8_t length);

/**
 * @brief Parse data
 *
 * Parse data of the NMEA0183_t struct and store them in the
 * registered_sensor_t struct corresponding to the correct sensor that has
 * sent the sentence.
 * This function calls the parse function defined in the sensor_t struct.
 * If the sensor is not registered, the data are not parsed.
 *
 * @param *nmea0183 pointer to NMEA0183_t struct
 */
void NMEA0183_ParseData(NMEA0183_t *nmea0183);

/**
 * @brief Print data
 *
 * Print data of the NMEA0183_t struct by calling the print function
 * defined in the sensor_t struct.
 * If the sensor is not registered, the data are not printed.
 *
 * @param *nmea0183 pointer to NMEA0183_t struct
 */
void NMEA0183_PrintData(NMEA0183_t *nmea0183);

#endif // NMEA0183_H
