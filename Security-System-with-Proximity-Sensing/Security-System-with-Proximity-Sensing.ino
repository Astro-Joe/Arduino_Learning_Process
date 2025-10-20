#include <LiquidCrystal.h>
#include <HCSR04.h>

//---LCD pins Initialization---
// RS, E, D4, D5, D6, D7
LiquidCrystal lcd(2, 3, 4, 5, 6, 7);

//---HCSRO4 Sensor Initialization---
// Trig_pin and Echo pin
HCSR04 hc(8, 9);

void setup() {
  //---Initializing Serial monitor and LCD
  Serial.begin(9600);
  lcd.begin(16, 2);

  char screen_1[] = "Security System";
  for (int i = 0; i < screen_1.length(); i++){
    lcd.cursor();
    lcd.print(screen_1[i]);
    delay(200);
  }
  delay(1000);
  String screen_2 = "System Init...";
  for (int i = 0; i < screen_2.length(); i++){
    while (i >= 10 || i <= 13){
      lcd.cursor();
      lcd.print(screen_2[i]);
    }
    lcd.cursor();
    lcd.print(screen_2[i]);
    delay(200);
  }
} 

void loop() {
  
}