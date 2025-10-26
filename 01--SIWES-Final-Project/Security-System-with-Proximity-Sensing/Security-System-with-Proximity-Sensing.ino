  #include <SD.h>
  #include <SPI.h>
  #include <RTClib.h>
  #include <Keypad.h>
  #include <LiquidCrystal.h>
  #include <HCSR04.h>

  const unsigned long interval = 300UL;
  bool condition_check = true;
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
    {'*', '0', '@'}
  };
  unsigned char row_pins[rows] = {A1, A5, A3, A4};
  unsigned char column_pins[columns] = {A2, 0, 1};

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


  //---Option Menu---
  void option_menu() {
    lcd.setCursor(0, 0);
    lcd.print("Press a number");
    lcd.setCursor(0, 1);
    lcd.print("to select      ");
    delay(2000); 

    lcd.clear();

    lcd.setCursor(0, 0);
    lcd.print("1. New Password");
    delay(1000);
    lcd.setCursor(0, 1);
    lcd.print("2. Login");
  }


  //---Escape Key---
  bool escape_key_normal() {
    char key = keypad.getKey(); 
    if (key == '0') {
      option_menu();
      return true;
    }
    return false;
  }

  void escape_key_module() {
    char key = keypad.getKey(); 
    if (key == '*') {
      lcd.clear();
      condition_check = false;
    }
  }



  //---Loading Animation Screen---
  bool loading_animation(unsigned char col, unsigned char row) {
    unsigned long last_check = 0;
    unsigned char case_number = 0; 
    unsigned char cycle = 0;   
    while (cycle < 3) {
      unsigned long now = millis();
      if (now - last_check >= interval){
        last_check = now;
        lcd.setCursor(col, row);      
        switch (case_number) {
          case 0:
            lcd.print("."); break;
          case 1:
            lcd.print(".."); break;
          case 2:
            lcd.print("..."); break;
          case 3:
            lcd.print("   "); break;
        }
        case_number++;
        if (case_number >=4) {
          cycle++;
          case_number = 0;
        }
      }
      if (escape_key_normal()) {
        return true;
      }
      delay(1);  
    }
    return false;
  }
        

  //---System Config screen---
  bool system_config(){
    display("System Init");
    lcd.setCursor(0, 1);
    lcd.print("Press 0 to skip");
    if (loading_animation(11, 0)) {
      return true;
    }

    //  ();

    lcd.clear();
    
    //---RTC Initialization---
    display("RTC Init");
    lcd.setCursor(0, 1);
    lcd.print("Press 0 to skip");
    if (loading_animation(8, 0)) {
      return true;
    }
    //  ();
    lcd.clear();

    if (!rtc.begin()) {
      lcd.print("RTC Failed");
      lcd.setCursor(0, 1);
      lcd.print("Press * to skip");
      //Serial.println("RTC FAIL - halting");
      while (condition_check) {
        escape_key_module();    
      } // Halt if RTC not found
    }
    else {
    display("RTC Configured   ");
    lcd.clear();
    }

    //---SD card Initialaization---
    condition_check = true;
    pinMode(CS_Pin, OUTPUT);
    digitalWrite(CS_Pin, HIGH);
    display("SD Init");
    lcd.setCursor(0, 1);
    lcd.print("Press 0 to skip");
    if (loading_animation(7, 0)) {
      return true;
    }
    lcd.clear();

    if (!SD.begin(CS_Pin)) {
      lcd.print("SD Failed");
      lcd.setCursor(0, 1);
      lcd.print("Press * to skip");
      //Serial.println("SD FAIL - halting");
      while (condition_check) {
        escape_key_module();
      } // Halt if RTC not found
    }
    else {
      display("SD Configured   ");
      lcd.clear();
    }  
    return false;
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

    //system_config();

    if (system_config()) {
      return;
    }

    lcd.clear();

    option_menu();    
    delay(300);                                                                                                                                                                                                       
  } 

  void loop() { 
    distance = hc.dist();
    char key = keypad.getKey(); 

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


    // after 6000 ms -> follow the distance rule
    if (distance <= 45.0 && distance > 6.0) {
      lcd.display();
      digitalWrite(lcd_backlight, BACKLIGHT_ON_LEVEL);   // ON
      /*lcd.setCursor(0, 1);
      lcd.print("LCDbacklight ON!"); */ //Debugging purpose
    } 
    else if (distance < 5.0) {
      digitalWrite(lcd_backlight, BACKLIGHT_OFF_LEVEL);  // OFF
      lcd.noDisplay();
      /*lcd.setCursor(0, 1);
      lcd.print("                  "); */ //Debugging purpose
    }
    else {
      lcd.noDisplay();
      digitalWrite(lcd_backlight, BACKLIGHT_OFF_LEVEL);
      /*lcd.setCursor(0, 1);
      lcd.print("                  "); */ //Debugging purpose
    }
    

    if (key) {

      unsigned char num = key - '0'; // converts from ASCII value to the real value
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