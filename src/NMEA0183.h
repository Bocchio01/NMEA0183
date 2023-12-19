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
#define MAX_FIELDS_COUNT 20
#define MAX_SENSORS 10

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

// typedef struct
// {
// 	uint8_t length;
// 	char buffer[SENTENCE_MAX_LEGTH];
// } buffer_t;

typedef struct
{
	uint8_t length;
	char buffer[SENTENCE_MAX_LEGTH];
	sensor_t talkerID;
	uint8_t numberOfFields;
	char *fields[MAX_FIELDS_COUNT];
	uint8_t checksum;
	bool isChecksumValid;
} sentence_t;

typedef struct
{
	comunication_stage_t stage;
	comunication_status_t status;
} comunication_t;

struct NMEA0183_t
{
	sentence_t *sentence;
	comunication_t *comunication;
	uint8_t numRegisteredSensors;
	registered_sensor_t *registeredSensors[MAX_SENSORS];
};

NMEA0183_t *NMEA0183_Init();
sentence_t *NMEA0183_InitSentence();
comunication_t *NMEA0183_InitComunication();

void NMEA0183_Reset(NMEA0183_t *nmea0183);
void NMEA0183_Reset_Sentence(sentence_t *sentence);
void NMEA0183_Reset_Comunication(comunication_t *comunication);

void NMEA0183_RegisterSensor(NMEA0183_t *nmea0183, registered_sensor_t sensor_value);

void NMEA0183_Update(NMEA0183_t *nmea0183, uint8_t c);
void NMEA0183_AddToBuffer(NMEA0183_t *nmea0183, uint8_t c);
void NMEA0183_AnalyzeData(NMEA0183_t *nmea0183);
void NMEA0183_GetTalkerID(NMEA0183_t *nmea0183);
void NMEA0183_GetFields(NMEA0183_t *nmea0183);
void NMEA0183_GetChecksum(NMEA0183_t *nmea0183);
uint8_t NMEA0183_ComputeChecksum(char *buffer, uint8_t length);

void NMEA0183_ParseData(NMEA0183_t *nmea0183);
void NMEA0183_PrintData(NMEA0183_t *nmea0183);

#endif // NMEA0183_H
