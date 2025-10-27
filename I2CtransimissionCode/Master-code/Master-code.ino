#include <Wire.h>

#define SLAVE_ADDR 8

void setup() {
  Wire.begin();        // Master mode
  //Serial.begin(9600);
  //Serial.println("Master ready!");
}

void loop() {
  // Send '1' to turn ON LED
  //Serial.println("Sending LED ON");
  Wire.beginTransmission(SLAVE_ADDR);
  Wire.write('1');
  Wire.endTransmission();
  delay(2000);

  // Send '0' to turn OFF LED
  ///Serial.println("Sending LED OFF");
  Wire.beginTransmission(SLAVE_ADDR);
  Wire.write('0');
  Wire.endTransmission();
  delay(2000);
}
