#include <Keypad.h>
#include <LiquidCrystal.h>

// LCD pins: (RS, E, D4, D5, D6, D7)
const int RS = 12, E = 11, D4 = 5, D5 = 4, D6 = 3, D7 = 2;
LiquidCrystal lcd(RS, E, D4, D5, D6, D7);

const byte ROWS = 4;
const byte COLS = 4;

// Define keypad layout
char keys[ROWS][COLS] = {
  {'1','2','3','+'},
  {'4','5','6','-'},
  {'7','8','9','*'},
  {'C','0','=','/'}   // You can replace C with '.' if you want decimals
};

byte rowPins[ROWS] = {9, 8, 7, 6};
byte colPins[COLS] = {A3, A2, A1, A0}; // Use analog pins if you need more digital pins

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

String num1 = "";
String num2 = "";
char operation;
bool secondNumber = false;

void setup() {
  lcd.begin(16, 2);
  lcd.setCursor(0,0);
  lcd.print("Arduino Calc");
  delay(1500);
  lcd.clear();
}

void loop() {
  char key = keypad.getKey();
  
  if (key) {
    if ((key >= '0' && key <= '9') || key == '.') { // allow decimals
      if (!secondNumber) {
        num1 += key;
        lcd.setCursor(0,0);
        lcd.print(num1);
      } else {
        num2 += key;
        lcd.setCursor(0,1);
        lcd.print(num2);
      }
    }
    else if (key == '+' || key == '-' || key == '*' || key == '/') {
      operation = key;
      secondNumber = true;
      lcd.setCursor(15,0);
      lcd.print(operation);
    }
    else if (key == '=') {
      float n1 = num1.toFloat();
      float n2 = num2.toFloat();
      float result = 0;

      if (operation == '+') result = n1 + n2;
      else if (operation == '-') result = n1 - n2;
      else if (operation == '*') result = n1 * n2;
      else if (operation == '/') {
        if (n2 != 0) result = n1 / n2;
        else {
          lcd.clear();
          lcd.print("Error: Div by 0");
          delay(2000);
          resetCalc();
          return;
        }
      }

      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Result:");
      lcd.setCursor(0,1);
      lcd.print(result, 4); // show up to 4 decimals
      delay(4000);
      resetCalc();
    }
    else if (key == 'C') { // clear
      resetCalc();
    }
  }
}

void resetCalc() {
  num1 = "";
  num2 = "";
  operation = '\0';
  secondNumber = false;
  lcd.clear();
}
