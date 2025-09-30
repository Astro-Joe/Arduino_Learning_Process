#include <Keypad.h>

// ----- Keypad Setup -----
const unsigned char ROWS = 4; 
const unsigned char COLS = 4; 
char keys[ROWS][COLS] = {
	{'1','2','3','+'},   // S1, S2, S3, S4
	{'4','5','6','-'},   // S5–S8
	{'7','8','9','*'},   // S9–S12
	{'C','0','=','/'}    // S13–S16
}; 
unsigned char rowPins[ROWS] = {2, 3, 4, 5}; 
unsigned char colPins[COLS] = {6, 7, 8, 9}; 
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

// ----- 7-Segment Setup -----
unsigned char segPins[7] = {10,11,12,13,A2,A3,A4}; // a,b,c,d,e,f,g
unsigned char digitPins[2] = {A0, A1}; // Control for two digits

unsigned char numbers[11] = {
	0b00111111, // 0
	0b00000110, // 1
	0b01011011, // 2
	0b01001111, // 3
	0b01100110, // 4
	0b01101101, // 5
	0b01111101, // 6
	0b00000111, // 7
	0b01111111, // 8
	0b01101111, // 9
	0b01000000  // '-'
}; // Array of the display digits in Bitmasks.

unsigned char Error[2] = {
  0b01111001, // 'E'
  0b01010000  // 'r'
}; // Print 'Er' when digit is greater than 99.



char num1 = 0, num2 = 0, result = 0; // Variables to hold the first and second number with the result respectively.
char op = 0;
bool enteringNum1 = true; // State flag for switching between num1 and num2
bool NegNum1 = false; // default mode 
bool NegNum2 = false; // default mode
bool errorState  = false; // default mode

// ----- Display Function -----
void PositiveNumber(int value) {
	int tens = (abs(value) / 10) % 10;
	int unit = abs(value) % 10;
	
	// Show tens digit
	digitalWrite(digitPins[0], LOW);
	digitalWrite(digitPins[1], HIGH);
	// For looping through the bitmask
	for(int i = 0; i < 7; i++){
		digitalWrite(segPins[i], (numbers[tens] >> i) & 1);
	}
	delay(5);

	// Show unit digit
	digitalWrite(digitPins[0], HIGH);
	digitalWrite(digitPins[1], LOW);
	// For looping through the bitmask
	for(int i = 0; i < 7; i++){
		digitalWrite(segPins[i], (numbers[unit] >> i) & 1);
	}
	delay(5);
}

// ----Display Error Message----
void ErrorMessage(){
  if(errorState){
    digitalWrite(digitPins[0], LOW);
    digitalWrite(digitPins[1], HIGH); 

    for(int i = 0; i < 7; i++){
      digitalWrite(segPins[i], (Error[0] >> i) & 1);
    }
    delay(5);

    digitalWrite(digitPins[0], HIGH);
    digitalWrite(digitPins[1], LOW); 

    for(int i = 0; i < 7; i++){
      digitalWrite(segPins[i], (Error[1] >> i) & 1);
    }
    delay(5);

    return;
  }
}

// ---- Display Negative number----
void NegativeNumber(int value){
  int absVal = abs(value);
  if(value < 0 && absVal < 10){
  // Show minus sign on first digit
    digitalWrite(digitPins[0], LOW);   // enable left digit
    digitalWrite(digitPins[1], HIGH);  // disable right digit
    for (int i = 0; i < 7; i++) {
      digitalWrite(segPins[i], (numbers[10] >> i) & 1);
    }
    delay(5);
    
    // Show digit on second digit
    digitalWrite(digitPins[0], HIGH);   // enable right digit
    digitalWrite(digitPins[1], LOW);  // disable right digit
    for (int i = 0; i < 7; i++) {
      digitalWrite(segPins[i], (numbers[absVal] >> i) & 1);
    }  
    delay(5); 
  }
}


void setup(){
  for(int i = 0; i < 7; i++) pinMode(segPins[i], OUTPUT);
  for(int i = 0; i < 2; i++) pinMode(digitPins[i], OUTPUT);
  Serial.begin(9600);
}

void loop(){
  char key = keypad.getKey();

  if (key) {
    Serial.println(key);
    
    //----For Numbers 0 - 9----
    if (isdigit(key)) {
      if (enteringNum1) {
        num1 = num1 * 10 + (key - '0');

        if(num1 > 99){
          errorState = true;
          ErrorMessage();
        }
        PositiveNumber(num1)
      }
    
      else {
        num2 = num2 * 10 + (key - '0');

        if(num2 > 99){
          errorState = true;
          ErrorMessage();
        }
        PositiveNumber(num2)
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

    else if (key == '-') {
      if (enteringNum1 && num1 == 0) {
        num1 = -0;  // mark as negative
      } 
      else if (!enteringNum1 && num2 == 0) {
        num2 = -0;  // mark as negative
      } 
      else {
        op = '-';   // subtraction operator
        enteringNum1 = false;
      }
    }
    

      else if (key == 'C') {
        num1 = num2 = result = 0;
        enteringNum1 = true;
        op = 0;
      }
    }
    PositiveNumber(result);
  }
 }

