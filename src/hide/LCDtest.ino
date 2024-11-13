#include <LiquidCrystal.h>





void setup(){


LiquidCrystal lcd(16, 17, 32, 14, 22, 21);
//RS E D4 D5 D6 D7
lcd.begin(16, 2);
// rows cols
lcd.print("hello world");
}


void loop(){


}
