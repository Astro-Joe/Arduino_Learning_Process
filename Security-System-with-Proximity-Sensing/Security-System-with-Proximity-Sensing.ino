#include <LiquidCrystal.h>
#include <HCSR04.h>

//---LCD pins Initialization---
// RS, E, D4, D5, D6, D7
LiquidCrystal lcd(2, 3, 4, 5, 6, 7);

//---HCSRO4 Sensor Initialization---
// Trig_pin and Echo pin
HCSR04 hc(8, 9);

unsigned char distance_pin = A0;

void setup() {
  //---Initializing Serial monitor and LCD
  Serial.begin(9600);
  lcd.begin(16, 2);
  lcd.cursor();

  String screen_1 = "Security System_";
  for (unsigned char i = 0; i < screen_1.length(); i++){
    lcd.print(screen_1[i]);
    delay(150);
  }
  delay(1000);
  lcd.clear();

  String screen_2 = "System Init";
  for (unsigned char i = 0; i < screen_2.length(); i++){
    lcd.print(screen_2[i]);
    delay(150);
  }
  
  //---Loading Animation---
  for (unsigned char i = 0; i < 3; i++){
    String screen_3 = "...";
    for (unsigned char j = 0; j < screen_3.length(); j++){
      lcd.print(screen_3[j]);
      delay(200);
    }
  lcd.setCursor(11, 0);
  }
  lcd.clear();
} 

void loop() {
  int distance = hc.dist();
  Serial.println(distance);
  if (distance <= 30){
    digitalWrite(distance_pin, HIGH);
  }
  delay(1000);


}