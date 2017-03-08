#include <SoftwareSerial.h>
#include <LiquidCrystal.h>

LiquidCrystal lcd(4, 5, 10, 11, 12, 13);
SoftwareSerial mySerial(0, 1);

void setup(){
  mySerial.begin(9600);

  while(mySerial.available() <= 0){
    delay(10);
  }

}

void loop(){
  lcd.print(mySerial.readString());

  while(mySerial.available() <= 0){
    delay(10);
  }

  lcd.clear();
  lcd.setCursor(0, 0);
}
