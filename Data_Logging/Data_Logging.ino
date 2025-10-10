#include <LiquidCrystal.h> // Library for the LCD
#include <DHT.h> // Library for the DHT11 sensor 
#include <SD.h>  // Library for the SD card module 
#include <SPI.h> // Library for SerialPeripheralInterface (High speed devices)
#include <RTClib.h> // Library for the RealTimeClock module 
#include <Wire.h> // Library for Inter-IntergratedCircuit (I2C) communication. 

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
  lcd.begin(16, 2); // Initializes LCD
  dht.begin(); // Initializes DHT11 sensor 
  Wire.begin();

//------ Sets all LEDs to OUTPUT-----
  pinMode(LED_TEMP_HIGH, OUTPUT);
  pinMode(LED_TEMP_NORMAL, OUTPUT);
  pinMode(LED_HUMID_LOW, OUTPUT);
  pinMode(LED_HUMID_NORMAL, OUTPUT);
  pinMode(BUZZER, OUTPUT);

  lcd.print("Weather Station"); // Prints the statement on the LCD
  delay(2000); // Waits for 2 seconds
  lcd.clear();  // Clears the LCD screen

    // --- SD CARD INIT ---
  bool SD_check = SD.begin(CS_PIN); // Initializes the SD card, returns
    // a bool depending on whether it has been initialized or not.

  if (!SD_check) { // Returns True if the SD card fails to initialize
    lcd.print("SD init failed!");
    while (1); // Let the program be stuck in an infinite loop as far
    // the condition is True
  }
  lcd.print("SD Ready");
  delay(2000); // Waits for 2 seconds 
  lcd.clear(); // Clelars LCD screen

    // --- RTC INIT ---
  bool RTC_check = rtc.begin(); // Initializes the RTC module, returns
    // a bool depending on whether it has been initialized or not.

  if (!RTC_check) { // Returns True if RTC fails to initialize 
    lcd.print("RTC failed!");
    while (1); // Let the program be stuck in an infinite loop as far
    // the condition is True
  }

  if (rtc.lostPower()) { // Checks if the RTC clock chip has lost power 
    // while the arduino was off
    lcd.print("RTC Resetting");
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__))); // Resets clock chip
    // to the date and time the code was last compiled.
    // NTP can be used(on an ESP32) to fetch real time from the internet.
  }

   // --- CREATE/OPEN LOG FILE ---
  if(!SD.exists("data.txt")){ // Checks if the file exists, so that the headers would not 
    // be written after every startup.
    logFile = SD.open("data.txt", FILE_WRITE); // Opens a file with the specified file name 
      // and initializes it for writing.
    if (logFile) {
      logFile.println("Date,Time,Temp(C),Humidity(%)");  // Column names for sensor data
      logFile.close(); // Finishes writing data, Frees system resources, closes the 
      // file to keep the SD card stable.
    }
  }
}

// ---------------- LOOP ----------------
void loop() {
  float humidity = dht.readHumidity(); // reads humimdity
  float temperature = dht.readTemperature(); // reads temperature 

  if (isnan(humidity) || isnan(temperature)) { // Checks if the sensor returned as 
    // valdid value
    lcd.setCursor(0, 0);
    lcd.print("Sensor Error!   ");
    return;
  }

  // --- DISPLAY ON LCD ---
  lcd.setCursor(0, 0);
  lcd.print("Temp: ");
  lcd.print(temperature, 4); // Prints the temperature in 4 decimal places
  lcd.print("C");

  lcd.setCursor(0, 1);
  lcd.print("Hum: ");
  lcd.print(humidity, 2); // Prints humdity in 2 decimal places
  lcd.print("%");

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
