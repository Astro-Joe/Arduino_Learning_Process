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

unsigned long lastSecondMillis = 0;
const unsigned long SECOND_INTERVAL = 1000UL;

bool loggedThisMinute = false; // prevents multiple writes in the same minute

// --- Averaging buffer (1 minute) ---
const uint8_t SAMPLE_COUNT = 60;        // number of seconds to average
int samples[SAMPLE_COUNT];              // raw ADC samples (0..1023)
uint16_t sampleIndex = 0;
uint32_t sampleSum = 0;
bool bufferFilled = false;

// reference voltage for ADC conversion (set 5.0 or 3.3)
const float REF_VOLTAGE = 5.0;
const float DIVIDER_FACTOR = 2.0; // adjust if your divider ratio differs

// Logging retry parameters
const uint8_t MAX_OPEN_RETRIES = 5;
const unsigned long RETRY_DELAY_MS = 120; // small delay between retries

// Helper: attempt to append a line to file with retries and SD re-init fallback
bool safeAppendLine(const char *filename, const char *line) {
  for (uint8_t attempt = 0; attempt < MAX_OPEN_RETRIES; ++attempt) {
    // Ensure CS is high before calling SD.open to avoid stray low
    digitalWrite(CS_PIN, HIGH);
    File f = SD.open(filename, FILE_WRITE);
    if (f) {
      // success
      f.println(line);
      f.close();
      Serial.print("SD: wrote (attempt ");
      Serial.print(attempt + 1);
      Serial.println(")");
      return true;
    } else {
      Serial.print("SD: open failed (attempt ");
      Serial.print(attempt + 1);
      Serial.println(")");
      // try small delay to let card settle
      delay(RETRY_DELAY_MS);
      // attempt to re-init SD bus (sometimes module needs re-init)
      Serial.println("SD: retrying SD.begin()");
      if (!SD.begin(CS_PIN)) {
        Serial.println("SD.begin() retry FAILED");
      } else {
        Serial.println("SD.begin() retry OK");
      }
    }
  }
  // all attempts failed
  Serial.println("SD: ALL OPEN ATTEMPTS FAILED");
  return false;
}

void setup() {
  Serial.begin(115200);
  while (!Serial) { /* wait for Serial on some boards */ }

  lcd.begin(16, 2);
  lcd.print("Solar Logger");
  delay(1200);
  lcd.clear();

  // Initialize RTC
  lcd.print("RTC Init...");
  if (!rtc.begin()) {
    lcd.clear();
    lcd.print("RTC FAIL!");
    Serial.println("RTC FAIL - halting");
    while (1); // Halt if RTC not found
  }
  lcd.clear();

  // Prepare SD: ensure CS is OUTPUT and set HIGH (required on some modules)
  pinMode(CS_PIN, OUTPUT);
  digitalWrite(CS_PIN, HIGH);

  lcd.print("SD Init...");
  if (!SD.begin(CS_PIN)) {
    lcd.clear();
    lcd.print("SD FAIL!");
    Serial.println("Initial SD.begin() FAILED");
    // don't halt — continue to allow diagnostics, but file ops will fail until fixed
  } else {
    Serial.println("SD.begin() OK");
  }
  lcd.clear();

  // Create header if file doesn't exist
  if (!SD.exists("solar_voltage.csv")) {
    Serial.println("Header missing: creating solar_voltage.csv");
    if (!safeAppendLine("solar_voltage.csv", "Date,Time,Voltage(V)")) {
      Serial.println("Failed to create header on solar_voltage.csv");
    }
  } else {
    Serial.println("Header exists");
  }

  lcd.print("System Ready");
  delay(900);
  lcd.clear();

  // Initialize timer
  lastSecondMillis = millis();

  // Initialize sample buffer with first reading to avoid 0-average
  int first = analogRead(VOLTAGE_PIN);
  for (uint8_t i = 0; i < SAMPLE_COUNT; ++i) {
    samples[i] = first;
    sampleSum += first;
  }
  bufferFilled = true;
  sampleIndex = 0;

  Serial.println("Setup complete");
}

void loop() {
  unsigned long nowMillis = millis();

  // update every second
  if (nowMillis - lastSecondMillis >= SECOND_INTERVAL) {
    lastSecondMillis += SECOND_INTERVAL; // keep drift small

    DateTime now = rtc.now(); // read RTC (DS3231)

    // Read raw ADC and update circular buffer / running sum
    int raw = analogRead(VOLTAGE_PIN);
    // Update running sum: subtract oldest sample, add new
    sampleSum -= samples[sampleIndex];
    samples[sampleIndex] = raw;
    sampleSum += samples[sampleIndex];

    sampleIndex++;
    if (sampleIndex >= SAMPLE_COUNT) {
      sampleIndex = 0;
      bufferFilled = true;
    }

    // Calculate average ADC (use SAMPLE_COUNT once filled, else use number filled)
    uint8_t usedCount = bufferFilled ? SAMPLE_COUNT : sampleIndex;
    if (usedCount == 0) usedCount = 1; // safety

    float avgADC = ((float)sampleSum) / usedCount;

    // Convert average ADC to voltage using reference and divider
    float avgVoltage = (avgADC * REF_VOLTAGE / 1023.0) * DIVIDER_FACTOR;

    // Format strings
    char timeStr[9];
    snprintf(timeStr, sizeof(timeStr), "%02u:%02u:%02u", now.hour(), now.minute(), now.second());

    char dateStr[12];
    snprintf(dateStr, sizeof(dateStr), "%02u/%02u/%04u", now.day(), now.month(), now.year());

    // Display top row: averaged voltage (1-minute moving average)
    lcd.setCursor(0, 0);
    lcd.print("Volt: ");
    // Print voltage directly (LiquidCrystal supports float print)
    lcd.setCursor(6, 0);
    lcd.print(avgVoltage, 2);  // shows approx 'real' voltage
    lcd.print("V   ");         // pad to clear old chars

    // Display bottom row: time (seconds ticking)
    lcd.setCursor(0, 1);
    lcd.print(timeStr);
    lcd.print("   ");

    // Serial debug (optional)
    Serial.print(dateStr);
    Serial.print(",");
    Serial.print(timeStr);
    Serial.print(",");
    Serial.println(avgVoltage, 3);

    // ---- LOG INSTANTANEOUS VOLTAGE ONCE PER MINUTE ----
    if (now.second() == 0 && !loggedThisMinute) {
      // Small diagnostic: print a marker just before attempt
      Serial.println("=== Logging instant voltage NOW ===");

      // Read instantaneous ADC right now (this is the spike-capable value)
      int rawInstant = analogRead(VOLTAGE_PIN);
      float instVoltage = (rawInstant * REF_VOLTAGE / 1023.0) * DIVIDER_FACTOR;

      // Format instant voltage into a string safely (AVR)
      char voltBuf[12];
      dtostrf(instVoltage, 6, 3, voltBuf); // width=6, precision=3

      // CSV line: date,time,voltage
      char csvLine[64];
      snprintf(csvLine, sizeof(csvLine), "%s,%s,%s", dateStr, timeStr, voltBuf);

      // Attempt safe append with diagnostics
      bool ok = safeAppendLine("solar_voltage.csv", csvLine);
      if (!ok) {
        Serial.println("ERR: cannot open solar_voltage.csv after retries");
        // Optionally: try toggling CS, small delay, one final attempt
        digitalWrite(CS_PIN, HIGH);
        delay(200);
        if (SD.begin(CS_PIN)) {
          Serial.println("SD.begin() final attempt OK, trying append once more");
          File f = SD.open("solar_voltage.csv", FILE_WRITE);
          if (f) {
            f.println(csvLine);
            f.close();
            Serial.println("SD: final attempt succeeded");
          } else {
            Serial.println("SD: final open failed");
          }
        }
      }

      loggedThisMinute = true; // mark as logged for this minute
    }

    // Reset the flag once seconds != 0 so we can log again next minute
    if (now.second() != 0) {
      loggedThisMinute = false;
    }
  }

  // non-blocking: do other light tasks here if needed
}
