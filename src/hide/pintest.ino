#define mypin 4 

void setup()  {
  pinMode(mypin, OUTPUT);
}

void loop() {
 digitalWrite(mypin, HIGH);
 delay(500);
 digitalWrite(mypin, LOW);
 delay(500);
}
