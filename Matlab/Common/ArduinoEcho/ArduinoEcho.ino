/**
 * ArduinoEcho.ino
 *
 * @author Windex Team
 * @date 20/12/2023
 */

void setup()
{
  Serial.begin(4800);
}

void loop()
{
  if (Serial.available())
  {
    Serial.print(Serial.read());
  }
}