  #include <SD.h>
  #include <SPI.h>
  #include <RTClib.h>
  #include <Keypad.h>
  #include <LiquidCrystal.h>
  #include <HCSR04.h>

  unsigned long now = millis();
  unsigned long last_check = 0;
  const byte interval = 300;
  const bool condition_check = true;
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
  const byte rows = 4;
  const byte columns = 3;
  char keys [rows][columns] = {
    {'1', '2', '3'},
    {'4', '5', '6'},
    {'7', '8', '9'},
    {'*', '0', '@'}
  };
  byte row_pins[rows] = {A1, A5, A3, A4};
  byte column_pins[columns] = {A2, 0, 1};

  //---Keypad Initialization---
  Keypad keypad = Keypad(makeKeymap(keys), row_pins, column_pins, rows, columns);
  char key = keypad.getKey(); 

  //---RTC Initialization---
  RTC_DS3231 rtc;

  //---Display function---
  void display(String sentence) {
    for (byte i = 0; i < sentence.length(); i++){
    lcd.print(sentence[i]);
    delay(100);
    }
  }


  //---Option Menu---
  void option_menu() {
    lcd.setCursor(0, 0);
    lcd.print("Press a number");
    lcd.setCursor(0, 1);
    lcd.print("to select");
    delay(3000); 

    lcd.clear();

    lcd.setCursor(0, 0);
    lcd.print("1. New Password");
    delay(1000);
    lcd.setCursor(0, 1);
    lcd.print("2. Login");
  }


  //---Escape Key---
  void escape_key() {
    if (key) {
      byte esc = key - '0';
      if (esc == 0) {
        condition_check = false;
        option_menu();
        return;
      }
    }
  }


  //---Loading Animation Screen---
  void loading_animation(byte col, byte row){ 
    byte case_number = 0; 
    byte cycle = 0;   
    while (cycle < 3) {
      while (case_number < 3) {
        last_check = now;
        if (now - last_check >= interval){
          last_check = now;
          lcd.setCursor(col, row);      
          switch (case_number) {
            case 0:
              lcd.print(".");
              break;
            case 1:
              lcd.print("..");
              break;
            case 2:
              lcd.print("...");
              break;
          }
          case_number++;
          if (case_number >=3) {
          cycle++;
          case_number = 0;
        }
        escape_key();
      }
    }
  }
  

  //---System Config screen---
  void system_config() {
    display("System Init");
    lcd.setCursor(0, 1);
    lcd.print("Press 0 to skip");
    loading_animation(11, 0);
    escape_key();

    lcd.clear();
    
    //---RTC Initialization---
    display("RTC Init");
    lcd.setCursor(0, 1);
    lcd.print("Press 0 to skip");
    loading_animation(8, 0);
    escape_key();
    lcd.clear();

    if (!rtc.begin()) {
      lcd.print("RTC Failed");
      lcd.setCursor(0, 1);
      lcd.print("Press 0 to skip");
      //Serial.println("RTC FAIL - halting");
      while (condition_check) {
          escape_key();
      } // Halt if RTC not found
    }
    else {
    display("RTC Configured   ");
    lcd.clear();
    }

    //---SD card Initialaization---
    pinMode(CS_Pin, OUTPUT);
    digitalWrite(CS_Pin, HIGH);
    display("SD Init");
    lcd.setCursor(0, 1);
    lcd.print("Press 0 to skip");
    loading_animation(7, 0);
    lcd.clear();

    if (!SD.begin(CS_Pin)) {
      lcd.print("SD Failed");
      lcd.setCursor(0, 1);
      lcd.print("Press 0 to skip");
      //Serial.println("SD FAIL - halting");
      while (condition_check) {
        escape_key();
      } // Halt if RTC not found
    }
    else {
      display("SD Configured   ");
      lcd.clear();
    }   
  }


  void setup() {

    //---Initializing Serial monitor and LCD
    //Serial.begin(9600);
    lcd.begin(16, 2);
    // lcd.cursor();
    lcd.noBlink();
    
    pinMode(lcd_backlight, OUTPUT);
    digitalWrite(lcd_backlight, HIGH);

    display("Security System");
    delay(2000);
    lcd.clear();

    system_config();

    lcd.clear();

    option_menu();                                                                                                                                                                                                           
  } 

  void loop() { 
    distance = hc.dist();

    //unsigned long now = millis();

    //---Debugging purpose--- 
    /*Serial.print("millis: "); 
    Serial.print(now); 
    Serial.print("   "); 

    Serial.print("distance: ");
    Serial.print(distance);
    Serial.println(); */

    /*lcd.setCursor(0, 0);
    lcd.print("DistKeypad:");
    lcd.print(distance);
    lcd.print("cm   "); */
    //---Debugging purpose--- 

    if (now < 23512UL) {
      digitalWrite(lcd_backlight, BACKLIGHT_ON_LEVEL);
    }
    else {
      // after 6000 ms -> follow the distance rule
      if (distance <= 45.0 && distance > 6.0) {
        digitalWrite(lcd_backlight, BACKLIGHT_ON_LEVEL);   // ON
        /*lcd.setCursor(0, 1);
        lcd.print("LCDbacklight ON!"); */ //Debugging purpose
      } 
      else if (distance < 5.0) {
        digitalWrite(lcd_backlight, BACKLIGHT_OFF_LEVEL);  // OFF
        /*lcd.setCursor(0, 1);
        lcd.print("                  "); */ //Debugging purpose
      }
      else {
        digitalWrite(lcd_backlight, BACKLIGHT_OFF_LEVEL);
        /*lcd.setCursor(0, 1);
        lcd.print("                  "); */ //Debugging purpose
      }
    }

    if (key) {

      byte num = key - '0'; // converts from ASCII value to the real value
      //Serial.println(key);
      if(num == 1){
        lcd.clear();
        lcd.print("Funct 1 Working!");
      }
      else if (num == 2) {
        lcd.clear();
        lcd.print("Funct 2 Working!");
      }
      else {
        lcd.clear();
        lcd.print("Enter a valid");
        lcd.setCursor(0,1);
        lcd.print("option!       ");
        delay(2000);
        lcd.clear();
        option_menu();
      }
    }
  //delay(300);
  }