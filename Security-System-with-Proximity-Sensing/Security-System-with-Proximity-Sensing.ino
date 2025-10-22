#include <SD.h>
#include <SPI.h>
#include <Wire.h>
#include <RTClib.h>
#include <Key.h>
#include <Keypad.h>
#include <LiquidCrystal.h>
#include <HCSR04.h>

const bool backlight_switch = false;
#define BACKLIGHT_ON_LEVEL  (backlight_switch ? LOW  : HIGH)
#define BACKLIGHT_OFF_LEVEL (backlight_switch ? HIGH : LOW)
//---SD Chip Select Pin---
#define CS_Pin 10

int lcd_backlight = A0;
int distance;

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
//unsigned char column_pins[columns] = {A5, 0, 1};

//---Keypad Initialization---
//Keypad keypad = Keypad(makeKeymap(keys), row_pins, column_pins, rows, columns);

//---RTC Initialization---
RTC_DS3231 rtc;

//---Display function---
void display(String sentence) {
  for (unsigned char i = 0; i < sentence.length(); i++){
  lcd.print(sentence[i]);
  delay(100);
  }
}

//---Loading Animation---
void loading_animation(unsigned char char_length){
  for (unsigned char i = 0; i < 3; i++) {
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

  display("Security System");
  delay(2000);
  lcd.clear();

  display("System Init");
  loading_animation(11);
  lcd.clear();
  

  display("RTC Init");
  loading_animation(8);
  lcd.clear();

  if (!rtc.begin()) {
    lcd.print("RTC Failed");
    Serial.println("RTC FAIL - halting");
    while (1); // Halt if RTC not found
  }
  else {
  display("RTC Configured   ");
  lcd.clear();
  }


  //---SD card Initialaization---
  pinMode(CS_Pin, OUTPUT);
  digitalWrite(CS_Pin, HIGH);
  display("SD Init");
  loading_animation(7);
  lcd.clear();

  if (!SD.begin(CS_Pin)) {
    lcd.print("SD Failed");
    Serial.println("SD FAIL - halting");
    while (1); // Halt if RTC not found
  }
  else {
  display("SD Configured   ");
  lcd.clear();
  }

} 

void loop() { 
  distance = hc.dist();

  unsigned long now = millis();
  Serial.print("millis: ");
  Serial.print(now);
  Serial.print("   ");

  Serial.print("distance: ");
  Serial.print(distance);
  Serial.println();

  lcd.setCursor(0, 0);
  lcd.print("DistKeypad:");
  lcd.print(distance);
  lcd.print("cm   ");

  unsigned long current_millis = millis();
  if (current_millis < 18497UL) {
    digitalWrite(lcd_backlight, BACKLIGHT_ON_LEVEL);
  }
  else {
    // after 6000 ms -> follow the distance rule
    if (distance <= 29.0 && distance > 6.0) {
      digitalWrite(lcd_backlight, BACKLIGHT_ON_LEVEL);   // ON
      lcd.setCursor(0, 1);
      lcd.print("LCDbacklight ON!");
    } 
    else if (distance < 5.0) {
      digitalWrite(lcd_backlight, BACKLIGHT_OFF_LEVEL);  // OFF
      lcd.setCursor(0, 1);
      lcd.print("                  ");
    }
    else {
      digitalWrite(lcd_backlight, BACKLIGHT_OFF_LEVEL);
      lcd.setCursor(0, 1);
      lcd.print("                  ");
    }
  }
  delay(300);
}