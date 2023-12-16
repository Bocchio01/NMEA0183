/**
 * @author Windex Team
 * @date 16/12/2023
 */

#ifndef WINDEX_H
#define WINDEX_H

#include <stdbool.h>
#include <stdint.h>

#define SENTENCE_MAX_LEGTH 82 + 1
#define SENTENCE_CHARACTER_START 0x24
#define SENTENCE_CHARACTER_DELIMITER 0x2C
#define SENTENCE_CHARACTER_END 0x00
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

typedef enum
{
	DATA_TYPE_WIND,
	DATA_TYPE_TEMPERATURE,
	DATA_TYPE_UNKNOWN
} data_type_t;

typedef enum
{
	WIND_UNITS_KNOTS,
	WIND_UNITS_METERSECOND,
	WIND_UNITS_KILOMETERSECOND
} wind_units_t;

typedef enum
{
	CV7_STATUS_AVAILABLE,
	CV7_STATUS_ALARM
} cv7_status_t;

typedef enum
{
	TEMPERATURE_UNITS_CELSIUS,
	TEMPERATURE_UNITS_FAHRENHEIT
} temperature_units_t;

typedef struct
{
	uint8_t length;
	char buffer[SENTENCE_MAX_LEGTH];
	data_type_t talkerID;
	uint8_t checksum;
} sentence_t;

typedef struct
{
	float angle;
	char reference;
	float speed;
	wind_units_t unit;
	cv7_status_t CV7_status;
} data_wind_t;

typedef struct
{
	float temperature;
	temperature_units_t unit;
} data_wind_temperature_t;

data_wind_t data_wind;
data_wind_temperature_t data_wind_temperature;
sentence_t sentence;
comunication_stage_t comunication_stage = COMUNICATION_STAGE_WAITING;
comunication_status_t comunication_status = COMUNICATION_STATUS_OK;

/**
 * Update the received data coming from the windex.
 *
 * @param c Value received from the windex.
 */
void update(uint8_t c);

/**
 * Add a character to the sentence buffer.
 *
 * @param c Character to add to the sentence buffer.
 */
void addToBuffer(uint8_t c);

/**
 * Reset to the initial state the system.
 * Used in case of error or to prepare the system for a new message.
 */
void reset();

/**
 * Get the talker ID of the sentence.
 */
void getTalkerID();

/**
 * Get the checksum of the sentence.
 */
void getChecksum();

/**
 * Select and call the correct function to fill the data structure.
 * The function is selected based on the talker ID of the sentence.
 */
void fillDataStructures();

/**
 * Fill the data_wind structure with the data received from the windex.
 *
 * @param sentence Full sentence received from the sensor.
 */
void fillDataWind();

/**
 * Fill the data_wind_temperature structure with the data received from the windex.
 *
 * @param sentence Full sentence received from the sensor.
 */
void fillDataWindTemperature();

/**
 * Compute the checksum of the sentence.
 * The checksum is computed as the XOR of all the characters between the $ and the *.
 *
 * @return The checksum of the sentence.
 */
uint8_t computeChecksum();

char *getMessage();
char *getSubString(int start, int finish, char string[]);

#endif
