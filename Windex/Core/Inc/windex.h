#ifndef __WINDEX_H
#define __WINDEX_H


#include <stdbool.h>
#include <stdint.h>

#define SENTENCE_MAX_LEGTH 82+1
#define SENTENCE_CHARACTER_START "$"
#define SENTENCE_CHARACTER_DELIMITER ","
#define SENTENCE_CHARACTER_END "\0"


typedef enum {
	COMUNICATION_STAGE_WAITING,
	COMUNICATION_STAGE_INCOMING,
	COMUNICATION_STAGE_DONE,
} comunication_stage_t;

typedef enum {
	COMUNICATION_STATUS_OK,
	COMUNICATION_STATUS_ERROR
} comunication_status_t;

typedef enum {
	DATA_TYPE_WIND,
	DATA_TYPE_TEMPERATURE,
	DATA_TYPE_UNKNOWN
} data_type_t;

// Wind related
typedef enum {
	WIND_UNITS_KNOTS,
	WIND_UNITS_METERSECOND,
	WIND_UNITS_KILOMETERSECOND
} wind_units_t;

typedef enum {
	CV7_STATUS_AVAILABLE,
	CV7_STATUS_ALARM
} cv7_status_t;

// Wind temperature related
typedef enum {
	TEMPERATURE_UNITS_CELSIUS,
	TEMPERATURE_UNITS_FAHRENHEIT
} temperature_units_t;

typedef struct {
	char data[SENTENCE_MAX_LEGTH];
	uint8_t length;
} sentence_t;

typedef struct {
	float angle;
	char reference;
	float speed;
	wind_units_t unit;
	cv7_status_t CV7_status;
	char checksum;
	bool isValid;
} data_wind_t;

typedef struct {
	float temperature;
	temperature_units_t unit;
	char checksum;
	bool isValid;
} data_wind_temperature_t;


/* Variable declaration */
data_wind_t data_wind;
data_wind_temperature_t data_wind_temperature;
sentence_t sentence;
comunication_stage_t comunication_stage = COMUNICATION_STAGE_WAITING;
comunication_status_t comunication_status = COMUNICATION_STATUS_OK;


/* Function declaration */
void update(char c);
void addToSentence(char c);
void reset();

data_type_t getDataType(sentence_t sentence);
void fillDataWind(sentence_t sentence);
void fillDataWindTemperature(sentence_t sentence);
// bool isValidData(data_wind_t data_wind);
// bool isValidData(data_wind_temperature_t data_wind_temperature);
// void printData(data_wind_t data_wind);
// void printData(data_wind_temperature_t data_wind_temperature);


#endif /* __WINDEX_H */
