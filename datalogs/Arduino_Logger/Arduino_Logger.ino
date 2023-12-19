void setup() {
  Serial.begin(4800);
}

void loop() {
  if (Serial.available()) {
    char data_rcvd = Serial.read();
    Serial.print(data_rcvd);
  }
}