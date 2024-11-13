//Changelog updated pin assignments to match color detector schematic v1.1 on 11/7 at 6:20p
//TODO add code to detect potentiometer and phototransistor

#include <LiquidCrystal.h>  // LCD library for text display
//#include <SD_MMC.h>  // SD library to manage SD card read/write operations.
#include <SdFat.h>
#include <SPI.h>
#include <sdios.h>

const int8_t DISABLE_CHIP_SELECT = -1;



#ifndef SDCARD_SS_PIN
const uint8_t SD_CS_PIN = SS;
#else   // SDCARD_SS_PIN
const uint8_t SD_CS_PIN = SDCARD_SS_PIN;
#endif  // SDCARD_SS_PIN


#if HAS_SDIO_CLASS
#define SD_CONFIG SdioConfig(FIFO_SDIO)
#elif ENABLE_DEDICATED_SPI
#define SD_CONFIG SdSpiConfig(SD_CS_PIN, DEDICATED_SPI, SD_SCK_MHZ(16))
#else  // HAS_SDIO_CLASS
#define SD_CONFIG SdSpiConfig(SD_CS_PIN, SHARED_SPI, SD_SCK_MHZ(16))
#endif  // HAS_SDIO_CLASS





#define RED_LED 33 //ESP5
#define GREEN_LED 27 //ESP6  // Pin for Green LED output to indicate green color detection.
#define BLUE_LED 12 //ESP6
#define YELLOW_LED 13 //ESP21  // Pin for Yellow LED output to indicate yellow color detection.

#define BUTTON_PIN 32  //ESP2
#define TOGGLE_SWITCH_PIN 14 //ESP1

#define PHOTOTRANSISTOR_PIN 1 //ESP14_A1
#define POTENTIOMETER_PIN 0 //ESP10_A0

//assign ESP32 pins to LCD pins in order RS(4) E(6) D4(11) D5(12) D6(13) D7(14)
//LiquidCrystal lcd(18,16,17,19,20,21);
//LiquidCrystal lcd(18,19,23,35,36,39);



//uc18->lcd4
//uc16->lcd6
//uc17->lcd11
//uc19->lcd12
//uc20->lcd13
//uc21->lcd14

void setup()//called automagically cuz arduino
{
   Serial.begin(9600);//baud rate must match serial terminal dummy
   Serial.println("serial started");
  // lcd.begin(16, 2);
   Serial.println("did the LCD kill us?");

//   if (!SD_MMC.begin()) {
      
      Serial.println("SD Card Mount Failed");
 //     lcd.print("SD Card Error");
      delay(2000);
 //     lcd.clear();
  //    return;  // Stop setup if SD card initialization fails
  } 


  Serial.println("SD Card Initialized");
  //lcd.print("Hello!");
  delay(1000);
  //lcd.clear();


   pinMode(BUTTON_PIN, INPUT_PULLUP);
   pinMode(TOGGLE_SWITCH_PIN, INPUT_PULLUP);
   //pinMode(BUTTON_PIN, INPUT);
   //pinMode(TOGGLE_SWITCH_PIN, INPUT);
//don't set pinmode for analog IO
//  pinMode(POTENTIOMETER_PIN, INPUT);
//  pinMode(PHOTOTRANSISTOR_PIN, INPUT);
  pinMode(YELLOW_LED, OUTPUT);
pinMode(GREEN_LED, OUTPUT);
pinMode(RED_LED, OUTPUT);
pinMode(BLUE_LED, OUTPUT);

   Serial.println("pins configured");

//with bottom 4 pins disabled the setup sequence finishes


   Serial.println("about to read toggle switch");
  if (digitalRead(TOGGLE_SWITCH_PIN)) {
     Serial.println("about to clear lcd(if)");
    //lcd.clear();
    //lcd.print("Training Mode");
    delay(2000);  // Show mode for 2 seconds
    //lcd.clear();
  } else {
         Serial.println("about to clear lcd(else)");
     //   lcd.clear();
     //   lcd.print("Sampling Mode");
        delay(2000);  // Show mode for 2 seconds
     //   lcd.clear();
  }
   Serial.println("end of setup reached");
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
      
  //    lcd.clear();
  //    lcd.print("toggle pos a");
      delay(2000);  // Show mode for 2 seconds
  //    lcd.clear();
    } else {
      digitalWrite(RED_LED, LOW);
      digitalWrite(GREEN_LED, LOW);
      digitalWrite(BLUE_LED, HIGH);
      digitalWrite(YELLOW_LED, HIGH);
      
  //    lcd.clear();
  //    lcd.print("toggle pos b");
      delay(2000);  // Show mode for 2 seconds
  //    lcd.clear();
  }

  
  if (!digitalRead(BUTTON_PIN))
  {
 //   lcd.clear();
 //   lcd.print("button press detected");
    delay(1000);  // Show confirmation for 1 second
 //   lcd.clear();
  }
}
