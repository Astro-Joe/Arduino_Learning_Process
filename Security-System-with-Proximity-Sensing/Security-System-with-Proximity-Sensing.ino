#include <SD.h>
#include <SPI.h>
#include <Wire.h>
#include <RTClib.h>
#include <Key.h>
#include <Keypad.h>
#include <LiquidCrystal.h>
#include <HCSR04.h>

const bool backlight_switch = true;
#define BACKLIGHT_ON_LEVEL  (backlight_switch ? LOW  : HIGH)
#define BACKLIGHT_OFF_LEVEL (backlight_switch ? HIGH : LOW)
#define CS_Pin 10;

int lcd_backlight = A0;
float distance;

//---LCD pins Initialization---
// RS, E, D4, D5, D6, D7
LiquidCrystal lcd(2, 3, 4, 5, 6, 7);

//---HCSRO4 Sensor Initialization---
// Trig_pin and Echo pin
HCSR04 hc(8, 9);

//---Keypad Initialization---
const unsigned char rows = 4;
const unsigned char columns = 3;
char keys [rows][columns] = {
  {'1', '2', '3'},
  {'4', '5', '6'},
  {'7', '8', '9'},
  {'#', '0', '*'}
};
unsigned char row_pins[rows] = {A1, A2, A3, A4};
unsigned char column_pins[columns] = {A5, 0, 1};

//---Keypad Initialization---
Keypad keypad = Keypad(makeKeymap(keys), row_pins, column_pins, rows, columns);

//---RTC Initialization---
RTC_DS3231 rtc;

//---SD card Initialaization---

//---Loading Animation---
void loading_animation(unsigned char char_length){
  for (unsigned char i = 0; i < 4; i++) {
    lcd.setCursor(char_length , 0); // cycles between 3 dots
    lcd.print(".");
    delay(300);
    lcd.setCursor(char_length, 0);
    lcd.print("..");
    delay(300);
    lcd.setCursor(char_length, 0); // cycles between 3 dots
    lcd.print("...");
    delay(300);
    lcd.setCursor(char_length, 0); // cycles between 3 dots
    lcd.print("   ");
  }
  lcd.noBlink();
  delay(300);
}

void setup() {

  //---Initializing Serial monitor and LCD
  Serial.begin(9600);
  lcd.begin(16, 2);
  // lcd.cursor();
  lcd.noBlink();
  
  pinMode(lcd_backlight, OUTPUT);
  digitalWrite(lcd_backlight, HIGH);

  String screen_1 = "Security System_";
  for (unsigned char i = 0; i < screen_1.length(); i++){
    lcd.print(screen_1[i]);
    delay(100);
  }
  delay(1000);
  lcd.clear();

  String screen_2 = "System Init";
  for (unsigned char i = 0; i < screen_2.length(); i++){
    lcd.print(screen_2[i]);
    delay(100);
  }
  loading_animation(10);
  lcd.clear();
  
  lcd.print("RTC Init");
  loading_animation(8);
  lcd.clear();
  if (!rtc.begin()) {
    lcd.clear();
    lcd.print("RTC FAIL!");
    Serial.println("RTC FAIL - halting");
    while (1); // Halt if RTC not found
  } 
} 

void loop() { 
  //Serial.println(millis());
  int distance = hc.dist();
  Serial.println(distance);
  unsigned long current_millis = millis();
  if (current_millis < 6000UL) {
    digitalWrite(lcd_backlight, BACKLIGHT_ON_LEVEL);
  }
  else {
    // after 6000 ms -> follow the distance rule
    if (distance >= 29.0) {
      digitalWrite(lcd_backlight, BACKLIGHT_ON_LEVEL);   // ON
    } else {
      digitalWrite(lcd_backlight, BACKLIGHT_OFF_LEVEL);  // OFF
    }
  }

  
  //---Checking if a key has been pressed---
  char key = keypad.getKey();
  if(key != NO_KEY) {
    Serial.println(key);
  }

// delay(1000); 
}
