#include <LiquidCrystal.h>

// LCD pin connections: (RS, E, D4, D5, D6, D7)
LiquidCrystal lcd(7, 6, 5, 4, 3, 2);

const int trigPin = 9;
const int echoPin = 10;

long duration;
float distance;

void setup() {
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  lcd.begin(16, 2);
  lcd.setCursor(0, 0);
  lcd.print("Distance Meter");
  delay(2000);
  lcd.clear();
}

void loop() {
  // Trigger ultrasonic pulse
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);

  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  // Read echo
  duration = pulseIn(echoPin, HIGH);

  // Calculate distance in cm
  distance = duration * 0.034 / 2;

  // Display on LCD
  lcd.setCursor(0, 0);
  lcd.print("Distance:     ");
  lcd.setCursor(0, 1);
  lcd.print(distance);
  lcd.print(" cm");

  delay(500);
}
