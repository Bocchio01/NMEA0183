/**
 * @file Sender.ino
 * @brief Sketch for the ESP32 sender board.
 *
 * @author Windex Team
 * @date 17/05/2024
 */

#include <esp_now.h>
#include <WiFi.h>

extern "C"
{
#include "src/NMEA0183/NMEA0183.h"
#include "src/NMEA0183/sensors/mwv.h"
#include "src/NMEA0183/sensors/xdr.h"
}

#define DATA_MAX_SIZE 250 - sizeof(sensor_ID_t)

typedef struct
{
  sensor_ID_t sensorID;
  uint8_t data[DATA_MAX_SIZE];
} esp_struct_t;

uint8_t broadcastAddress[] = {0x08, 0xD1, 0xF9, 0x27, 0xF3, 0xA0}; // MAC Address of receiver
esp_now_peer_info_t peerInfo;

NMEA0183_t *nmea0183;
uint16_t dataLength;

void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status)
{
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery success" : "Delivery fail");
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

  esp_now_register_send_cb(OnDataSent);

  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;

  if (esp_now_add_peer(&peerInfo) != ESP_OK)
  {
    Serial.println("Failed to add peer");
    return;
  }

  // NMEA0183 library and sensor registration
  nmea0183 = NMEA0183_Init();
  NMEA0183_RegisterSensor(nmea0183, MWV_Init());
  NMEA0183_RegisterSensor(nmea0183, XDR_Init());
}

void loop()
{
  if (Serial.available())
  {
    NMEA0183_CharacterHandler(nmea0183, Serial.read());

    if (NMEA0183_IsDataReady(nmea0183))
    {

      esp_struct_t esp_struct;
      esp_struct.sensorID = NMEA0183_GetSensorID(nmea0183);

      switch (esp_struct.sensorID)
      {

      case MWV:
        dataLength = sizeof(MWV_data_t);
        break;

      case XDR:
        dataLength = sizeof(XDR_data_t);
        break;

      default:
        Serial.println("SensorID unknown.");
        break;
      }

      memcpy(esp_struct.data, (uint8_t *)NMEA0183_GetDataAddress(nmea0183), dataLength);

      esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *)&esp_struct, sizeof(esp_struct));
      if (result != ESP_OK)
      {
        Serial.println("Error sending data");
      }

      NMEA0183_Reset(nmea0183);
    }
  }
}