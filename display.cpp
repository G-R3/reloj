#include "display.h"
#include "Arduino.h"

Display::Display(LiquidCrystal& dp)
  : lcd_(dp) {}

void Display::begin() {
  lcd_.begin(16, 2);
  lcd_.clear();
  lcd_.setCursor(0, 0);
}

void Display::renderMenu(int selectedIndex) {
  lcd_.setCursor(0, 0);
  if (selectedIndex == 0) {
    lcd_.print(">");
  } else {
    lcd_.print(" ");
  }
  lcd_.print("Start");

  lcd_.setCursor(0, 1);
  if (selectedIndex == 1) {
    lcd_.print(">");
  } else {
    lcd_.print(" ");
  }
  lcd_.print("Config");
}

void Display::renderConfig(int selectedIndex) {
  lcd_.setCursor(0, 0);
  if (selectedIndex == 0) {
    lcd_.print(">");
  } else {
    lcd_.print(" ");
  }
  lcd_.print("5/3");

  lcd_.setCursor(0, 1);
  if (selectedIndex == 1) {
    lcd_.print(">");
  } else {
    lcd_.print(" ");
  }
  lcd_.print("10/5");
}

void Display::renderTimer(int minutes, int seconds, bool isFocused, bool isPaused) {
  lcd_.setCursor(0, 0);

  if (isFocused) {
    lcd_.print("Focus");
  } else {
    lcd_.print("Break");
  }

  lcd_.setCursor(0, 1);
  if (minutes < 10) lcd_.print("0");
  lcd_.print(minutes);
  lcd_.print(":");
  if (seconds < 10) lcd_.print("0");
  lcd_.print(seconds);
  lcd_.print(" ");


  lcd_.setCursor(5, 1);
  if (isPaused) {
    lcd_.print(" PAUSED");
  } else {
    lcd_.print("       ");
  }
}

void Display::clear() {
  lcd_.clear();
  lcd_.setCursor(0, 0);
}

void Display::renderHold(const char* label, unsigned long elapsedMs, unsigned long holdMs) {
  constexpr uint8_t barWidth = 14; // defines the space between the brackets [               ]
  unsigned long clampedElapsed = elapsedMs > holdMs ? holdMs : elapsedMs;
  uint8_t filled = holdMs == 0 ? barWidth : (clampedElapsed * barWidth) / holdMs;

  lcd_.setCursor(0, 0);
  lcd_.print("Hold: ");
  lcd_.print(label);
  lcd_.print("      ");
  lcd_.setCursor(0, 1);
  lcd_.print("[");
  for (uint8_t i = 0; i < barWidth; ++i) {
    lcd_.print(i < filled ? "#" : " ");
  }
  lcd_.print("]");
}