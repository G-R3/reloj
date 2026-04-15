#pragma once

#include <LiquidCrystal.h>

class Display {
public:
  Display(LiquidCrystal& dp);
  void begin();
  void renderTimer(int minutes, int seconds, bool isFocused, bool isPaused);
  void renderMenu(int selectedIndex);
  void renderConfig(int selectedIndex);
  void renderHold(const char* label, unsigned long elapsedMs, unsigned long holdMs);
  void clear();

private:
  LiquidCrystal& lcd_;
};