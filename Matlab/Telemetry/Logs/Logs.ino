/**
 * Logs.ino
 *
 * @brief Logs data coming from RX pin to serial monitor
 *
 * This can be usefull to observe data sent by an external sensor communicating
 * via UART protocol (i.e. Windex)
 *
 * Connections:
 * - RX pin: TX pin of the external sensor
 *
 * @author Windex Team
 * @date 20/12/2023
 */

void setup() {
  Serial.begin(4800);
}

void loop() {
  if (Serial.available()) {
    Serial.print(Serial.read());
  }
}