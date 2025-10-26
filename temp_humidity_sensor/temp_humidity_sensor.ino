#include <LiquidCrystal.h>
#include <DHT.h>

// LCD setup: (RS, E, D4, D5, D6, D7)
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

// DHT setup
#define DHTPIN 7       // Data pin connected to Arduino pin 7
#define DHTTYPE DHT11  // Change to DHT22 if using DHT22
DHT dht(DHTPIN, DHTTYPE);

void setup() {
  lcd.begin(16, 2);  // 16x2 LCD
  dht.begin();
  lcd.print("Temp & Humidity");
  delay(2000);
  lcd.clear();
}

void loop() {
  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature(); // Celsius
  // float tempF = dht.readTemperature(true); // Fahrenheit if needed

  if (isnan(humidity) || isnan(temperature)) {
    lcd.setCursor(0,0);
    lcd.print("Sensor Error!");
    return;
  }

  lcd.setCursor(0, 0);
  lcd.print("Temp: ");
  lcd.print(temperature);
  lcd.print(" C");

  lcd.setCursor(0, 1);
  lcd.print("Hum:  ");
  lcd.print(humidity);
  lcd.print(" %");

  delay(2000); // update every 2 seconds
}
