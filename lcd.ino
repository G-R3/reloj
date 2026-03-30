#include "LiquidCrystal.h"
#include "app.h"

const int rs = 12;
const int enable = 11;
const int d4 = 5;
const int d5 = 4;
const int d6 = 3;
const int d7 = 2;

LiquidCrystal lcd(rs, enable, d4, d5, d6, d7);
App app(lcd);

void setup() {
  Serial.begin(9600);
  app.begin(millis());
}

void loop() {
  app.update();
}