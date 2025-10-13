#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <LiquidCrystal.h>
#include <RTClib.h>

#define VOLTAGE_PIN A0
#define CS_PIN 10

// LCD pins: RS, EN, D4, D5, D6, D7
LiquidCrystal lcd(7, 8, 9, 5, 6, A3);

// DS3231 RTC instance
RTC_DS3231 rtc;

File dataFile;

void setup() {
  Serial.begin(9600);
  lcd.begin(16, 2);
  lcd.print("Solar Logger");
  delay(2000);
  lcd.clear();

  // Initialize RTC
  lcd.print("RTC Init...");
  delay(1000);
  lcd.clear();

  if (!rtc.begin()) {
    lcd.clear();
    lcd.print("RTC FAIL!");
    while (1); // Halt if RTC not found
  }

  // Uncomment once to set time, then comment back
  // rtc.adjust(DateTime(2025, 10, 13, 10, 30, 0)); // year, month, day, hour, min, sec
  // Or set from compile time:
  

  int SD_init = SD.begin(CS_PIN);
  // Initialize SD card
  lcd.print("SD Init...");
  delay(1000);
  if (!SD_init) {
    lcd.clear();
    lcd.print("SD FAIL!");
    while (1);
  }

  rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  lcd.clear();
  lcd.print("SD Ready");
  delay(1000);
  lcd.clear();

  // Create/open CSV log file
  dataFile = SD.open("solarV.csv", FILE_WRITE);
  if (dataFile) {
    dataFile.println("Date,Time,Voltage(V)");
    dataFile.close();
  }

  lcd.print("System Ready");
  delay(1500);
  lcd.clear();
}

void loop() {
  // Get current time
  DateTime now = rtc.now();

  // Read and convert voltage
  int rawValue = analogRead(VOLTAGE_PIN);
  float voltage = (rawValue * 5.0 / 1023.0) * 2.0; // Adjust factor for voltage divider

  // Format date and time
  char dateStr[15];
  sprintf(dateStr, "%02d/%02d/%04d", now.day(), now.month(), now.year());

  char timeStr[10];
  sprintf(timeStr, "%02d:%02d:%02d", now.hour(), now.minute(), now.second());

  // Build CSV line
  String dataString = String(dateStr) + "," + String(timeStr) + "," + String(voltage, 2);

  // Write to SD card
  dataFile = SD.open("solarV.csv", FILE_WRITE);
  if (dataFile) {
    dataFile.println(dataString);
    dataFile.close();
  }

  // Display on LCD
  lcd.setCursor(0, 0);
  lcd.print("Volt: ");
  lcd.print(voltage, 2);
  lcd.print("V   ");

  lcd.setCursor(0, 1);
  lcd.print(timeStr);
  lcd.print("   ");

  // Serial monitor output
  Serial.println(dataString);

  delay(60000); // Log every 60 seconds
}
