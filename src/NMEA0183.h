#ifndef NMEA0183_H
#define NMEA0183_H

#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include "sensors_handler.h"

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
// struct fields_t: Declared in sensors_handler.h;

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
	sensor_t *sensor[MAX_SENSORS];
} registered_sensor_t;

typedef struct
{
	sentence_t *sentence;
	comunication_t *comunication;
	registered_sensor_t *registeredSensor;
} NMEA0183_t;

NMEA0183_t *NMEA0183_Init();
sentence_t *NMEA0183_InitSentence();
comunication_t *NMEA0183_InitComunication();
registered_sensor_t *NMEA0183_InitRegisteredSensor();

void NMEA0183_Reset(NMEA0183_t *nmea0183);
void NMEA0183_ResetSentence(sentence_t *sentence);
void NMEA0183_ResetComunication(comunication_t *comunication);
void NMEA0183_ResetRegisteredSensor(registered_sensor_t *registeredSensor);

void NMEA0183_RegisterSensor(NMEA0183_t *nmea0183, sensor_t sensor_value);

void NMEA0183_Update(NMEA0183_t *nmea0183, uint8_t c);
void NMEA0183_AddToBuffer(NMEA0183_t *nmea0183, uint8_t c);
void NMEA0183_AnalyzeData(NMEA0183_t *nmea0183);
void NMEA0183_GetSensorID(NMEA0183_t *nmea0183);
void NMEA0183_GetFields(NMEA0183_t *nmea0183);
void NMEA0183_GetChecksum(NMEA0183_t *nmea0183);
uint8_t NMEA0183_ComputeChecksum(char *buffer, uint8_t length);

void NMEA0183_ParseData(NMEA0183_t *nmea0183);
void NMEA0183_PrintData(NMEA0183_t *nmea0183);

#endif // NMEA0183_H
