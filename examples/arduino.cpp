
#include <Arduino.h>

void setup() {
  Serial.begin(9600);
}

void loop() {
  Serial.println("test UART: 9600ps");
  delay(1000);
  Serial.begin(76800);
  Serial.println("test UART: 76800bps");
  delay(1000);
}
