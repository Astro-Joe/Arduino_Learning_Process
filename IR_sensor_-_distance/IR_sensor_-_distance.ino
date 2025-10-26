#include <LiquidCrystal.h>

// LCD pin connections: (RS, E, D4, D5, D6, D7)
LiquidCrystal lcd(7, 6, 5, 4, 3, 2);

const int irPin = A0;  // IR sensor connected to analog pin A0
int sensorValue;
float voltage;
float distance;  // in cm (approx)

void setup() {
  lcd.begin(16, 2);
  lcd.setCursor(0, 0);
  lcd.print("IR Distance");
  delay(2000);
  lcd.clear();
}

void loop() {
  // Read analog value
  sensorValue = analogRead(irPin);
  voltage = sensorValue * (5.0 / 1023.0);

  // Convert voltage to distance (approx, depends on sensor model!)
  // For Sharp GP2Y0A21 / MH-Sensor IR type:
  if (voltage > 0.4) {
    distance = 27.86 / (voltage - 0.42);  // empirical formula
  } else {
    distance = 80; // out of range
  }

  // Display result
  lcd.setCursor(0, 0);
  lcd.print("Distance:     ");
  lcd.setCursor(0, 1);
  lcd.print(distance);
  lcd.print(" cm   ");

  delay(500);
}
