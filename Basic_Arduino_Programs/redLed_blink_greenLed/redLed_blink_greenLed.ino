int redLed = 8;
int greenLed = 9;
int button = 2;

bool lastButton = LOW;

void setup() {
  // put your setup code here, to run once:
  pinMode(redLed, OUTPUT);
  pinMode(greenLed, OUTPUT); 
  pinMode(button, INPUT);
}

void loop() {
  bool currentButton = digitalRead(button);

  if(currentButton == HIGH && lastButton == LOW){
    
    digitalWrite(greenLed, LOW);
    
    for(int i =0; i < 3; i++){
      digitalWrite(redLed, HIGH);
      delay(300);
      digitalWrite(redLed, LOW);
      delay(300);
    }

    digitalWrite(greenLed, HIGH);

  }

  lastButton = currentButton;

}



