/**
 * @file Receiver.ino
 * @brief Sketch for the ESP32 receiver board.
 *
 * @author Windex Team
 * @date 17/05/2024
 */

#include <esp_now.h>
#include <WiFi.h>

// https://github.com/sandeepmistry/arduino-CAN/tree/master
#include "src/arduino-CAN/src/CAN.h"

extern "C"
{
#include "src/NMEA0183/sensors/mwv.h"
#include "src/NMEA0183/sensors/xdr.h"
}

#define DATA_MAX_SIZE 250 - sizeof(sensor_ID_t)

typedef struct
{
  sensor_ID_t sensorID;
  uint8_t data[DATA_MAX_SIZE];
} esp_struct_t;

MWV_data_t MWV_data;
XDR_data_t XDR_data;

void sendCANData(const uint8_t ID, const uint8_t *data, size_t length)
{
  const size_t maxPayload = 8;
  size_t bytesRemaining = length;
  size_t offset = 0;

  while (bytesRemaining > 0)
  {
    size_t packetSize = (bytesRemaining > maxPayload) ? maxPayload : bytesRemaining;

    CAN.beginPacket(ID);
    CAN.write(data + offset, packetSize);
    CAN.endPacket();

    offset += packetSize;
    bytesRemaining -= packetSize;
  }
}

void OnDataRecv(const uint8_t *mac, const uint8_t *incomingData, int len)
{

  esp_struct_t receivedData;
  memcpy(&receivedData, incomingData, sizeof(esp_struct_t));

  switch (receivedData.sensorID)
  {

  case MWV:
    memcpy(&MWV_data, receivedData.data, sizeof(MWV_data_t));
    sendCANData(0x02, (uint8_t *)&MWV_data, sizeof(MWV_data_t));
    // MWV_Printer(&MWV_data);
    break;

  case XDR:
    memcpy(&XDR_data, receivedData.data, sizeof(XDR_data_t));
    sendCANData(0x01, (uint8_t *)&XDR_data, sizeof(XDR_data_t));
    // XDR_Printer(&XDR_data);
    break;

  default:
    Serial.println("SensorID unknown.");
    break;
  }
}

void setup()
{
  Serial.begin(4800);

  WiFi.mode(WIFI_STA);

  // CAN.setPins(rx, tx);
  if (!CAN.begin(500E3))
  {
    Serial.println("Starting CAN failed!");
    return;
  }

  if (esp_now_init() != ESP_OK)
  {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  esp_now_register_recv_cb(OnDataRecv);
}

void loop() {}