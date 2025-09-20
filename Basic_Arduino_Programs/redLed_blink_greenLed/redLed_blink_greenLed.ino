// Initializing the arduino pins for all components.
int redLed = 8;
int greenLed = 9;
int button = 2;

bool lastButton = LOW; // setting default state of button.

void setup() {
  // Initializing pin modes for all components.
  pinMode(redLed, OUTPUT);
  pinMode(greenLed, OUTPUT); 
  pinMode(button, INPUT);
}

void loop() {
  bool currentButton = digitalRead(button); // Read button state 

  if(currentButton == HIGH && lastButton == LOW){
    
    digitalWrite(greenLed, LOW);
    
    for(int i =0; i < 3; i++){
      digitalWrite(redLed, HIGH);
      delay(300);
      digitalWrite(redLed, LOW);
      delay(300);
    } // Blinks the redLed thrice.

    digitalWrite(greenLed, HIGH); //Turns on the greenLed.

  }

  lastButton = currentButton; // Prevents loop from running again by setting
                              // lastButton to HIGH.

}



