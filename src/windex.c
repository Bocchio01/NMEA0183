#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>

#include "windex.h"

void reset()
{

	memset(sentence.buffer, 0, sizeof(sentence.buffer));
	sentence.length = 0;

	data_wind.angle = 0.0;
	data_wind.reference = 'N';
	data_wind.speed = 0.0;
	data_wind.unit = WIND_UNITS_KNOTS;
	data_wind.CV7_status = CV7_STATUS_AVAILABLE;

	data_wind_temperature.temperature = 0.0;
	data_wind_temperature.unit = TEMPERATURE_UNITS_CELSIUS;

	comunication_stage = COMUNICATION_STAGE_WAITING;
	comunication_status = COMUNICATION_STATUS_OK;
}

void update(uint8_t c)
{

	switch (comunication_stage)
	{
	case COMUNICATION_STAGE_WAITING:
		if (c == SENTENCE_CHARACTER_START)
		{
			comunication_stage = COMUNICATION_STAGE_INCOMING;
			addToBuffer(c);
		}
		break;

	case COMUNICATION_STAGE_INCOMING:
		addToBuffer(c);
		if (c == SENTENCE_CHARACTER_END)
		{
			comunication_stage = COMUNICATION_STAGE_ANALYZING;
			getTalkerID();
			getChecksum();

			if (comunication_status == COMUNICATION_STATUS_OK &&
				sentence.checksum == computeChecksum())
			{
				comunication_stage = COMUNICATION_STAGE_DONE;
				fillDataStructures();
			}
			else
			{
				comunication_status = COMUNICATION_STATUS_ERROR;
			}
		}

	case COMUNICATION_STAGE_DONE:
		break;

	default:
		reset();
		break;
	}

	if (comunication_status == COMUNICATION_STATUS_ERROR)
	{
		reset();
	}
}

void addToBuffer(uint8_t c)
{
	if (sentence.length >= SENTENCE_MAX_LEGTH)
	{
		comunication_status = COMUNICATION_STATUS_ERROR;
		return;
	}

	sentence.buffer[sentence.length] = c;
	sentence.length++;
}

void getTalkerID()
{

	char *commaPosition = strchr(sentence.buffer, SENTENCE_CHARACTER_DELIMITER);

	if (commaPosition != NULL)
	{
		char *talkerID = getSubString(1, commaPosition - sentence.buffer, sentence.buffer);

		if (strcmp(talkerID, "IIMWV") == 0)
		{
			sentence.talkerID = DATA_TYPE_WIND;
			return;
		}

		if (strcmp(talkerID, "WIXDR") == 0)
		{
			sentence.talkerID = DATA_TYPE_TEMPERATURE;
			return;
		}

		sentence.talkerID = DATA_TYPE_UNKNOWN;
		return;
	}

	comunication_status = COMUNICATION_STATUS_ERROR;
}

void getChecksum()
{

	char *asteriskPosition = strchr(sentence.buffer, SENTENCE_CHARACTER_CHECKSUM_START);

	if (asteriskPosition != NULL)
	{
		char *checksum = getSubString(asteriskPosition - sentence.buffer + 1, asteriskPosition - sentence.buffer + 3, sentence.buffer);
		sentence.checksum = (uint8_t)strtol(checksum, NULL, 16);
		return;
	}

	comunication_status = COMUNICATION_STATUS_ERROR;
}

void fillDataStructures()
{

	switch (sentence.talkerID)
	{
	case DATA_TYPE_WIND:
		fillDataWind();
		break;

	case DATA_TYPE_TEMPERATURE:
		fillDataWindTemperature();
		break;

	default:
		comunication_status = COMUNICATION_STATUS_ERROR;
		break;
	}
}

void fillDataWind()
{

	char *tokens[7];
	char *token = strtok(sentence.buffer, SENTENCE_CHARACTER_DELIMITER);
	int tokenCount = 0;

	while (token != NULL && tokenCount < 7)
	{
		tokens[tokenCount++] = token;
		token = strtok(NULL, ",");
	}

	if (tokenCount != 7)
	{
		comunication_status = COMUNICATION_STATUS_ERROR;
		return;
	}

	char *asteriskPosition = strchr(tokens[tokenCount], SENTENCE_CHARACTER_CHECKSUM_START);
	if (asteriskPosition != NULL)
	{
		token[tokenCount] = getSubString(0, asteriskPosition, tokens[tokenCount]);
	}

	data_wind.angle = (float)tokens[1][0];
	data_wind.reference = tokens[2][0];
	data_wind.speed = (float)tokens[3][0];

	switch (tokens[4][0])
	{
	case 'N':
		data_wind.unit = WIND_UNITS_KNOTS;
		break;
	case 'M':
		data_wind.unit = WIND_UNITS_METERSECOND;
		break;
	case 'K':
		data_wind.unit = WIND_UNITS_KILOMETERSECOND;
		break;
	}

	data_wind.CV7_status = tokens[5][0] == 'A' ? CV7_STATUS_AVAILABLE : CV7_STATUS_ALARM;
}

void fillDataWindTemperature()
{

	char *tokens[5];
	char *token = strtok(sentence.buffer, SENTENCE_CHARACTER_DELIMITER);
	int tokenCount = 0;

	while (token != NULL && tokenCount < 5)
	{
		tokens[tokenCount++] = token;
		token = strtok(NULL, ",");
	}

	if (tokenCount != 5)
	{
		comunication_status = COMUNICATION_STATUS_ERROR;
		return;
	}

	char *asteriskPosition = strchr(tokens[tokenCount], SENTENCE_CHARACTER_CHECKSUM_START);
	if (asteriskPosition != NULL)
	{
		token[tokenCount] = getSubString(0, asteriskPosition, tokens[tokenCount]);
	}

	data_wind_temperature.temperature = (float)tokens[1][0];

	switch (tokens[2][0])
	{
	case 'C':
		data_wind_temperature.unit = TEMPERATURE_UNITS_CELSIUS;
		break;
	case 'F':
		data_wind_temperature.unit = TEMPERATURE_UNITS_FAHRENHEIT;
		break;
	}
}

char *getMessage()
{
	char printableRepresentation[256];

	if (comunication_stage == COMUNICATION_STAGE_DONE && comunication_status == COMUNICATION_STATUS_OK)
	{
		snprintf(printableRepresentation, 256,
				 "Angle: %.2f\nReference: %c\nSpeed: %.2f\nUnit: %d\nCV7 Status: %d",
				 data_wind.angle,
				 data_wind.reference,
				 data_wind.speed,
				 data_wind.unit,
				 data_wind.CV7_status);
	}
	return printableRepresentation;
}

uint8_t computeChecksum()
{
	uint8_t checksum_value = 0;
	uint8_t index = 1; // Skip over the $ at the begining of the sentence

	while (index < sentence.length &&
		   sentence.buffer[index] != SENTENCE_CHARACTER_CHECKSUM_START &&
		   sentence.buffer[index] != SENTENCE_CHARACTER_END)
	{
		checksum_value ^= sentence.buffer[index];
		index++;
	}

	return checksum_value;
}

char *getSubString(int start, int finish, char string[])
{

	char *substring = malloc(sizeof(char) * (finish - start + 1));

	for (uint8_t i = start; i < finish; i++)
	{
		substring[i - start] = string[i];
	}
	substring += '\0';

	return substring;
}