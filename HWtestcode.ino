#include <LiquidCrystal.h>  // LCD library for text display
#include <SD_MMC.h>  // SD library to manage SD card read/write operations.

#define BUTTON_PIN 2  // Pin for button input to trigger actions in the program.
#define YELLOW_LED 14 //A0  // Pin for Yellow LED output to indicate yellow color detection.
#define GREEN_LED 15 //A1  // Pin for Green LED output to indicate green color detection.
#define RED_LED 16 //A2
#define BLUE_LED 17 //A3
#define PHOTOTRANSISTOR_PIN 18 //A4
#define TOGGLE_SWITCH_PIN 19 //A5
#define POTENTIOMETER_PIN 21 //A7

LiquidCrystal lcd(5, 18, 19, 16, 17, 21); //change to whatever pins we use


void setup()//called automagically cuz arduino
{
  Serial.begin(9600);//baud rate must match serial terminal dummy
    lcd.begin(16, 2);

  if (!SD_MMC.begin()) {
    Serial.println("SD Card Mount Failed");
    lcd.print("SD Card Error");
    delay(2000);
    lcd.clear();
    return;  // Stop setup if SD card initialization fails
  } 

  Serial.println("SD Card Initialized");
  lcd.print("Hello!");
  delay(1000);
  lcd.clear();

	pinMode(BUTTON_PIN, INPUT_PULLUP);
	pinMode(TOGGLE_SWITCH_PIN, INPUT_PULLUP);
	pinMode(POTENTIOMETER_PIN, INPUT);
	pinMode(PHOTOTRANSISTOR_PIN, INPUT);
	pinMode(YELLOW_LED, OUTPUT);
	pinMode(GREEN_LED, OUTPUT);
	pinMode(RED_LED, OUTPUT);
	pinMode(BLUE_LED, OUTPUT);


  if (digitalRead(TOGGLE_SWITCH_PIN)) {
    lcd.clear();
    lcd.print("Training Mode");
    delay(2000);  // Show mode for 2 seconds
    lcd.clear();
  } else {
        lcd.clear();
        lcd.print("Sampling Mode");
        delay(2000);  // Show mode for 2 seconds
        lcd.clear();
  }
}

void loop()
{
    // Check and update the mode (training or sampling) based on the toggle switch
  if (digitalRead(TOGGLE_SWITCH_PIN))
    {
      lcd.clear();
      lcd.print("toggle position a");
      delay(2000);  // Show mode for 2 seconds
      lcd.clear();
    } else {
      lcd.clear();
      lcd.print("toggle position b");
      delay(2000);  // Show mode for 2 seconds
      lcd.clear();
  }

  
  if (!digitalRead(BUTTON_PIN))
  {
    lcd.clear();
    lcd.print("button press detected");
    delay(1000);  // Show confirmation for 1 second
    lcd.clear();
  }
}