#include <LiquidCrystal.h>

// Initialize the library with the pins connected to LCD
// (RS, E, D4, D5, D6, D7)
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

void setup() {
  // Set up the LCD's number of columns and rows:
  lcd.begin(16, 2);

  // Print a message to the LCD
  lcd.print("Hello, World!");
}

void loop() {
  // Move cursor to second line
  lcd.setCursor(0, 1);
  lcd.print("Ifeoluwa D Senior");
}
