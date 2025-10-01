int segPins[7] = {2,3,4,5,6,7,8}; // 1d array containing the 7 pins
                                //  used in the display.
int digits[10][7] = {
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
}; // 10 by 7 array containing the digits to be displayed.
// rows represents the digits while columns represnts the pin segments

void setup() {
  for(int i = 0; i < 7; i++){
    pinMode(segPins[i], OUTPUT);
  } //iterates through the array segPins and assigns all pins as OUTPUT
}

void loop() {
  for(int n = 0; n < 10; n++){ //iterates through array digits
    for(int i = 0;i < 7; i++){ //iterates through every row in array digits
      digitalWrite(segPins[i], digits[n][i]); // Writes each segPins as the value in each row of array digits
      delay(1000);
    }
  }}
