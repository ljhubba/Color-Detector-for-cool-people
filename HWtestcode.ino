//Changelog updated pin assignments to match color detector schematic v1.1 on 11/7 at 6:20p
//TODO add code to detect potentiometer and phototransistor

#include <LiquidCrystal.h>  // LCD library for text display
#include <SD_MMC.h>  // SD library to manage SD card read/write operations.



#define RED_LED 6 //ESP5
#define GREEN_LED 7 //ESP6  // Pin for Green LED output to indicate green color detection.
#define BLUE_LED 8 //ESP6
#define YELLOW_LED 9 //ESP21  // Pin for Yellow LED output to indicate yellow color detection.

#define BUTTON_PIN 4  //ESP2
#define TOGGLE_SWITCH_PIN 3 //ESP1

#define PHOTOTRANSISTOR_PIN 23 //ESP14_A1
#define POTENTIOMETER_PIN 24 //ESP10_A0

//assign ESP32 pins to LCD pins in order RS(4) E(6) D4(11) D5(12) D6(13) D7(14)
LiquidCrystal lcd(18,16,17,19,20,21);
//uc18->lcd4
//uc16->lcd6
//uc17->lcd11
//uc19->lcd12
//uc20->lcd13
//uc21->lcd14

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
      digitalWrite(RED_LED, HIGH);
      digitalWrite(GREEN_LED, HIGH);
      digitalWrite(BLUE_LED, LOW);
      digitalWrite(YELLOW_LED, LOW);
      
      lcd.clear();
      lcd.print("toggle pos a");
      delay(2000);  // Show mode for 2 seconds
      lcd.clear();
    } else {
      digitalWrite(RED_LED, LOW);
      digitalWrite(GREEN_LED, LOW);
      digitalWrite(BLUE_LED, HIGH);
      digitalWrite(YELLOW_LED, HIGH);
      
      lcd.clear();
      lcd.print("toggle pos b");
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
