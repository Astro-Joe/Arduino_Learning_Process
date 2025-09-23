#include <Keypad.h>

// ----- Keypad Setup -----
const byte ROWS = 4; 
const byte COLS = 4; 
char keys[ROWS][COLS] = {
  {'1','2','3','+'},   // S1, S2, S3, S4
  {'4','5','6','-'},   // S5–S8
  {'7','8','9','*'},   // S9–S12
  {'C','0','=','/'}    // S13–S16
};
byte rowPins[ROWS] = {2, 3, 4, 5}; 
byte colPins[COLS] = {6, 7, 8, 9}; 
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

// ----- 7-Segment Setup -----
int segPins[7] = {10,11,12,13,A2,A3,A4}; // a,b,c,d,e,f,g
int digitPins[2] = {A0, A1}; // Control for two digits

int numbers[10][7] = {
  {1,1,1,1,1,1,0}, //0
  {0,1,1,0,0,0,0}, //1
  {1,1,0,1,1,0,1}, //2
  {1,1,1,1,0,0,1}, //3
  {0,1,1,0,0,1,1}, //4
  {1,0,1,1,0,1,1}, //5
  {1,0,1,1,1,1,1}, //6
  {1,1,1,0,0,0,0}, //7
  {1,1,1,1,1,1,1}, //8
  {1,1,1,1,0,1,1}  //9
};

int num1 = 0, num2 = 0, result = 0;
char op = 0;
bool enteringNum1 = true;

// ----- Display Function -----
void displayNumber(int value) {
  int tens = (value / 10) % 10;
  int ones = value % 10;

  // Show tens digit
  digitalWrite(digitPins[0], LOW);
  digitalWrite(digitPins[1], HIGH);
  for(int i=0;i<7;i++) digitalWrite(segPins[i], numbers[tens][i]);
  delay(5);

  // Show ones digit
  digitalWrite(digitPins[0], HIGH);
  digitalWrite(digitPins[1], LOW);
  for(int i=0;i<7;i++) digitalWrite(segPins[i], numbers[ones][i]);
  delay(5);
}

void setup(){
  for(int i=0;i<7;i++) pinMode(segPins[i], OUTPUT);
  for(int i=0;i<2;i++) pinMode(digitPins[i], OUTPUT);
  Serial.begin(9600);
}

void loop(){
  char key = keypad.getKey();

  if (key) {
    Serial.println(key);

    if (isdigit(key)) {
      if (enteringNum1) {
        num1 = num1 * 10 + (key - '0');
        if(num1 > 99) num1 = 99;
        result = num1;
      } else {
        num2 = num2 * 10 + (key - '0');
        if(num2 > 99) num2 = 99;
        result = num2;
      }
    }
    else if (key == '+' || key == '-' || key == '*' || key == '/') {
      op = key;
      enteringNum1 = false;
    }
    else if (key == '=') {
      if (op == '+') result = num1 + num2;
      else if (op == '-') result = num1 - num2;
      else if (op == '*') result = num1 * num2;
      else if (op == '/' && num2 != 0) result = num1 / num2;

      if(result > 99) result = 99; // fit into 2-digit display

      num1 = result;  // allow continuous calculation
      num2 = 0;
      enteringNum1 = true;
    }
    else if (key == 'C') {
      num1 = num2 = result = 0;
      enteringNum1 = true;
      op = 0;
    }
  }

  displayNumber(result);
}
