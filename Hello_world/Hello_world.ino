// Initializing arduino pins for the components 
int led = 13;
//int button = 2;

void setup() {
  // Initializing pin modes for the components 
  pinMode(led, OUTPUT);
  //pinMode(button, INPUT);
}

void loop() {
  // Sets led on when button is pressed(state is high assuming pull down is used)
  //if(digitalRead(button) == HIGH){
  digitalWrite(13, HIGH);
  delay(500);
  digitalWrite(13, LOW);
  delay(500);
} 


