/**
 * @file Receiver.ino
 * @brief Sketch for the ESP32 receiver board.
 *
 * @author Windex Team
 * @date 17/05/2024
 */

#include <esp_now.h>
#include <WiFi.h>

extern "C"
{
#include "src/NMEA0183/sensors/mwv.h"
#include "src/NMEA0183/sensors/xdr.h"
}

#define DATA_MAX_SIZE 250 - 32

typedef struct
{
  sensor_ID_t sensorID;
  uint8_t data[DATA_MAX_SIZE];
} esp_struct_t;

MWV_data_t MWV_data;
XDR_data_t XDR_data;

void OnDataRecv(const uint8_t *mac, const uint8_t *incomingData, int len)
{

  esp_struct_t receivedData;
  memcpy(&receivedData, incomingData, sizeof(esp_struct_t));

  switch (receivedData.sensorID)
  {

  case MWV:
    memcpy(&MWV_data, receivedData.data, sizeof(MWV_data_t));
    MWV_Printer(&MWV_data);
    break;

  case XDR:
    memcpy(&XDR_data, receivedData.data, sizeof(XDR_data_t));
    XDR_Printer(&XDR_data);
    break;

  default:
    Serial.println("SensorID unknown.");
    break;
  }
}

void setup()
{
  Serial.begin(4800);

  // WiFi setup and peer connection
  WiFi.mode(WIFI_STA);

  if (esp_now_init() != ESP_OK)
  {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  esp_now_register_recv_cb(OnDataRecv);
}

void loop() {}