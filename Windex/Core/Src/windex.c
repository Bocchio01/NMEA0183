#include <string.h>
#include <stdbool.h>
#include <stdlib.h>  // For atof function
#include <stdint.h>
#include "windex.h"


void reset() {

    // memset(sentence.data, 0, sizeof(sentence.data));
    sentence.data[0] = '\0';
	sentence.length = 0;

    data_wind.angle = 0.0;
    data_wind.reference = 'N';
    data_wind.speed = 0.0;
    data_wind.unit = WIND_UNITS_KNOTS;
    data_wind.CV7_status = CV7_STATUS_AVAILABLE;
    data_wind.checksum = '\0';
    data_wind.isValid = false;

    data_wind_temperature.temperature = 0.0;
    data_wind_temperature.unit = TEMPERATURE_UNITS_CELSIUS;
    data_wind_temperature.checksum = '\0';
    data_wind_temperature.isValid = false;

    comunication_stage = COMUNICATION_STAGE_WAITING;
    comunication_status = COMUNICATION_STATUS_OK;

}

void update(char c) {

	if(comunication_stage == COMUNICATION_STAGE_WAITING && c == SENTENCE_CHARACTER_START[0]) {
		comunication_stage = COMUNICATION_STAGE_INCOMING;
		addToSentence(c);
	}

	if(comunication_stage == COMUNICATION_STAGE_INCOMING) {
		if (c == '*') {
			addToSentence(SENTENCE_CHARACTER_DELIMITER[0]);
		}

		addToSentence(c);

		if (c == SENTENCE_CHARACTER_END[0]) {
			comunication_stage = COMUNICATION_STAGE_DONE;

			switch(getDataType(sentence))
			{
				case DATA_TYPE_WIND:
					fillDataWind(sentence);
					break;

				case DATA_TYPE_TEMPERATURE:
					fillDataWindTemperature(sentence);
					break;

				default:
					comunication_status = COMUNICATION_STATUS_ERROR;
			}

		}

	}

	if (comunication_stage == COMUNICATION_STAGE_DONE) {

	}

}


void addToSentence(char c) {
	sentence.data[sentence.length] = c;
	sentence.length++;
}


data_type_t getDataType(sentence_t sentence) {

    char* commaPosition = strchr(sentence.data, (int) SENTENCE_CHARACTER_DELIMITER);

    if (commaPosition != NULL) {

        uint8_t talkerIDLength = commaPosition - sentence.data;
        char talkerID[talkerIDLength + 1];
        strncpy(talkerID, sentence.data, talkerIDLength);
        talkerID[talkerIDLength] = '\0';


        if (strcmp(talkerID, "$IIMWV") == 0) {
            return DATA_TYPE_WIND;
        } else if (strcmp(talkerID, "$WIXDR") == 0) {
            return DATA_TYPE_TEMPERATURE;
        } else {
            return DATA_TYPE_UNKNOWN;
        }

    }

    return DATA_TYPE_UNKNOWN;

}


void fillDataWind(sentence_t sentence) {

    char *tokens[7];
    char *token = strtok(sentence.data, SENTENCE_CHARACTER_DELIMITER);
    int tokenCount = 0;

    // Store tokens in the array
    while (token != NULL && tokenCount < 7) {
        tokens[tokenCount++] = token;
        token = strtok(NULL, ",");
    }

    if (tokenCount == 7) {
    	    data_wind.angle = atof(tokens[1]);
			data_wind.reference = tokens[2][0]; // this is done to dereference from pointer
			data_wind.speed = atof(tokens[3]); // atof function to convert into float
			data_wind.unit = WIND_UNITS_KNOTS;
			data_wind.CV7_status = strcmp(tokens[5], "A") == 0 ? CV7_STATUS_AVAILABLE : CV7_STATUS_ALARM; // string compare instead of equating pointers
			data_wind.checksum = (char) token[6];
			data_wind.isValid = true;
    } else {
    	comunication_status = COMUNICATION_STATUS_ERROR;
    }

}

void fillDataWindTemperature(sentence_t sentence) {
	comunication_status = COMUNICATION_STATUS_ERROR;
}
