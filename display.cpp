#include "display.h"

Display::Display(LiquidCrystal& dp)
  : lcd(dp) {}

void Display::begin() {
  lcd.begin(16, 2);
  lcd.clear();
  lcd.setCursor(0, 0);
}

void Display::renderMenu(int selectedIndex) {
  lcd.setCursor(0, 0);
  if (selectedIndex == 0) {
    lcd.print(">");
  } else {
    lcd.print(" ");
  }
  lcd.print("Start");

  lcd.setCursor(0, 1);
  if (selectedIndex == 1) {
    lcd.print(">");
  } else {
    lcd.print(" ");
  }
  lcd.print("Config");
}

void Display::renderConfig(int selectedIndex) {
  lcd.setCursor(0, 0);
  if (selectedIndex == 0) {
    lcd.print(">");
  } else {
    lcd.print(" ");
  }
  lcd.print("5/3");

  lcd.setCursor(0, 1);
  if (selectedIndex == 1) {
    lcd.print(">");
  } else {
    lcd.print(" ");
  }
  lcd.print("10/5");
}

void Display::renderTimer(int minutes, int seconds, bool isFocused, bool isPaused) {
  lcd.setCursor(0, 0);

  if (isFocused) {
    lcd.print("Focus");
  } else {
    lcd.print("Break");
  }

  lcd.setCursor(0, 1);
  if (minutes < 10) lcd.print("0");
  lcd.print(minutes);
  lcd.print(":");
  if (seconds < 10) lcd.print("0");
  lcd.print(seconds);
  lcd.print(" ");


  lcd.setCursor(5, 1);
  if (isPaused) {
    lcd.print(" PAUSED");
  } else {
    lcd.print("       ");
  }
}

void Display::clear() {
  lcd.clear();
  lcd.setCursor(0, 0);
}
