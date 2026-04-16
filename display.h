#pragma once

#include <LiquidCrystal.h>

class Display {
public:
  Display(LiquidCrystal& dp);
  void begin();
  void renderTimer(int minutes, int seconds, bool isFocused, bool isPaused, long remainingMs, unsigned long sessionDurationMs);
  void renderMenu(int selectedIndex);
  void renderConfig(int selectedIndex);
  void renderHold(const char* label, unsigned long elapsedMs, unsigned long holdMs, bool isConfirmed);

private:
  void loadCustomChars();
  void writePaddedRow(uint8_t row, const char* text);
  void writeSmoothBar(uint8_t row, uint8_t col, uint8_t width, unsigned long elapsedMs, unsigned long totalMs);
  LiquidCrystal& lcd_;
};