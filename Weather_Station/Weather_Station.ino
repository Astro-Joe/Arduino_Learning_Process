#include <LiquidCrystal.h>
#include <DHT.h>
#include <SD.h>
#include <SPI.h>
#include <RTClib.h>
#include <Wire.h>

// ---------------- LCD SETUP ----------------
// (RS, E, D4, D5, D6, D7)
LiquidCrystal lcd(7, 6, 5, 4, 3, 2);

// ---------------- DHT SENSOR ----------------
#define DHTPIN 8
#define DHTTYPE DHT11     // Change to DHT22 if used
DHT dht(DHTPIN, DHTTYPE);

// ---------------- ALERT SYSTEM ----------------
#define LED_TEMP_HIGH 9       // Red LED → High temperature
#define LED_TEMP_NORMAL A0    // Green LED → Normal temperature
#define LED_HUMID_LOW A1      // Yellow LED → Low humidity
#define LED_HUMID_NORMAL A2   // Blue LED → Normal humidity or blinking for high
#define BUZZER A3             // Buzzer → Sound alert on abnormal readings

// ---------------- SD CARD ----------------
#define CS_PIN 10

// ---------------- RTC ----------------
RTC_DS3231 rtc;

// ---------------- THRESHOLDS ----------------
float maxTemp = 30.0;
float minTemp = 15.0;
float maxHum  = 70.0;
float minHum  = 30.0;

// ---------------- STATE VARIABLES ----------------
File logFile;
bool alert = false;
unsigned long previousBuzzMillis = 0;
const unsigned long buzzInterval = 500;  // 0.5 sec ON/OFF alarm interval
bool buzzerState = false;

// ---------------- SETUP ----------------
void setup() {
  lcd.begin(16, 2);
  dht.begin();
  Wire.begin();

  pinMode(LED_TEMP_HIGH, OUTPUT);
  pinMode(LED_TEMP_NORMAL, OUTPUT);
  pinMode(LED_HUMID_LOW, OUTPUT);
  pinMode(LED_HUMID_NORMAL, OUTPUT);
  pinMode(BUZZER, OUTPUT);

  lcd.print("Weather Station");
  delay(2000);
  lcd.clear();

  // --- SD CARD INIT ---
  if (!SD.begin(CS_PIN)) {
    lcd.print("SD init failed!");
    while (1);
  }
  lcd.print("SD Ready");
  delay(1000);
  lcd.clear();

  // --- RTC INIT ---
  if (!rtc.begin()) {
    lcd.print("RTC failed!");
    while (1);
  }

  if (rtc.lostPower()) {
    lcd.print("RTC Resetting");
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }

  // --- CREATE/OPEN LOG FILE ---
  logFile = SD.open("data.txt", FILE_WRITE);
  if (logFile) {
    logFile.println("Date,Time,Temp(C),Humidity(%)");
    logFile.close();
  }
}

// ---------------- LOOP ----------------
void loop() {
  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature(); // °C

  if (isnan(humidity) || isnan(temperature)) {
    lcd.setCursor(0, 0);
    lcd.print("Sensor Error!   ");
    return;
  }

  // --- DISPLAY ON LCD ---
  lcd.setCursor(0, 0);
  lcd.print("Temp:");
  lcd.print(temperature, 1);
  lcd.print("C   ");

  lcd.setCursor(0, 1);
  lcd.print("Hum: ");
  lcd.print(humidity, 1);
  lcd.print("%   ");

  alert = false;

  // Reset indicators
  digitalWrite(LED_TEMP_HIGH, LOW);
  digitalWrite(LED_TEMP_NORMAL, LOW);
  digitalWrite(LED_HUMID_LOW, LOW);
  digitalWrite(LED_HUMID_NORMAL, LOW);

  // ---------------- TEMPERATURE LOGIC ----------------
  if (temperature > maxTemp) {
    digitalWrite(LED_TEMP_HIGH, HIGH); // Red → high temperature
    alert = true;
  } else {
    digitalWrite(LED_TEMP_NORMAL, HIGH); // Green → normal
  }

  // ---------------- HUMIDITY LOGIC ----------------
  if (humidity < minHum) {
    digitalWrite(LED_HUMID_LOW, HIGH); // Yellow → low humidity
    alert = true;
  } else if (humidity > maxHum) {
    // Blink blue LED for high humidity
    if (millis() / 500 % 2 == 0)
      digitalWrite(LED_HUMID_NORMAL, HIGH);
    else
      digitalWrite(LED_HUMID_NORMAL, LOW);
    alert = true;
  } else {
    digitalWrite(LED_HUMID_NORMAL, HIGH); // Blue → normal humidity
  }

  // ---------------- BUZZER LOGIC ----------------
  unsigned long currentMillis = millis();
  if (alert) {
    if (currentMillis - previousBuzzMillis >= buzzInterval) {
      previousBuzzMillis = currentMillis;
      buzzerState = !buzzerState;
      digitalWrite(BUZZER, buzzerState ? HIGH : LOW);
    }
  } else {
    digitalWrite(BUZZER, LOW);
    buzzerState = false;
  }

  // ---------------- LOG TO SD ----------------
  DateTime now = rtc.now();
  logFile = SD.open("data.txt", FILE_WRITE);
  if (logFile) {
    logFile.print(now.year(), DEC); logFile.print("-");
    logFile.print(now.month(), DEC); logFile.print("-");
    logFile.print(now.day(), DEC);
    logFile.print(",");

    logFile.print(now.hour(), DEC); logFile.print(":");
    logFile.print(now.minute(), DEC); logFile.print(":");
    logFile.print(now.second(), DEC);
    logFile.print(",");

    logFile.print(temperature, 1);
    logFile.print(",");
    logFile.println(humidity, 1);
    logFile.close();

    // Blink green LED (preserve previous state)
    int prev = digitalRead(LED_TEMP_NORMAL);
    digitalWrite(LED_TEMP_NORMAL, prev == HIGH ? LOW : HIGH);
    delay(300);
    digitalWrite(LED_TEMP_NORMAL, prev);
  } else {
    lcd.setCursor(0, 0);
    lcd.print("SD Write Error  ");
  }

  // Wait 2 minutes before next reading
  delay(120000);
}
