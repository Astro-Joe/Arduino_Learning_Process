#include <Wire.h>

#define LED_PIN 13

void setup() {
  pinMode(LED_PIN, OUTPUT);
  Wire.begin(8);                // Join I2C bus with address #8
  Wire.onReceive(receiveEvent); // Register event handler
}

void loop() {
  delay(100);
}

void receiveEvent(int howMany) {
  while (Wire.available()) {     // Check if data is available
    char c = Wire.read();        // Read one character from master
    if (c == '1') {
      digitalWrite(LED_PIN, HIGH); // Turn LED ON
    } else if (c == '0') {
      digitalWrite(LED_PIN, LOW);  // Turn LED OFF
    }
  }
}
