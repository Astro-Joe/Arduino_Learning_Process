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
//---SD Chip Select Pin---
#define CS_Pin 10

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

//---Display function---
void display(String sentence) {
  for (unsigned char i = 0; i < sentence.length(); i++){
  lcd.print(sentence[i]);
  delay(100);
  }
}

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

  String screen_1 = "Security System";
  display(screen_1);
  delay(2000);
  lcd.clear();

  String screen_2 = "System Init";
  display(screen_2);
  loading_animation(11);
  lcd.clear();
  
  String screen_3 = "RTC Init";
  display(screen_3);
  loading_animation(8);
  lcd.clear();
  if (!rtc.begin()) {
    String screen_4 = "RTC Failed";
    for (unsigned char i = 0; i < 3; i++) { 
      display(screen_4);
      delay(200);
      lcd.clear();
    }
    lcd.print(screen_4);
    Serial.println("RTC FAIL - halting");
    while (1); // Halt if RTC not found
  }

  //---SD card Initialaization---
  pinMode(CS_Pin, OUTPUT);
  digitalWrite(CS_Pin, HIGH);
  
  String screen_5 = "SD Init";
  display(screen_5);
  loading_animation(7);
  lcd.clear();

  if (!SD.begin(CS_Pin)) {
    String screen_6 = "SD Failed";
    for (unsigned char i = 0; i < 3; i++) { 
      display(screen_6);
      delay(200);
      lcd.clear();
    }
    lcd.print(screen_6);
    Serial.println("SD FAIL - halting");
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
    if (distance >= 31.0) {
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
