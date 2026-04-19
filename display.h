#pragma once

#include <LiquidCrystal.h>

class Display {
public:
  Display(LiquidCrystal& dp);
  void begin();
  void renderTimer(int minutes, int seconds, bool isFocused, bool isPaused, long remainingMs, unsigned long sessionDurationMs);
  void renderMenu(int selectedIndex);
  void renderConfig(int selectedIndex, bool buzzerEnabled);

  // Show the hold-to-confirm overlay, then briefly switch to a confirmed state once
  // the hold has completed.
  // TODO: The confirmed state still flashes too quickly to feel intentional.
  void renderHold(const char* label, unsigned long elapsedMs, unsigned long holdMs, bool isConfirmed);

private:
  // Load the LCD's 8 custom-character slots with the bar segments and status icons
  // used across the redesigned screens.
  void loadCustomChars();
  // Write a full LCD row and pad the rest with spaces so old characters disappear.
  void writePaddedRow(uint8_t row, const char* text);
  // Draw a smoother progress bar by mixing empty cells with partially filled custom
  // characters.
  void writeSmoothBar(uint8_t row, uint8_t col, uint8_t width, unsigned long elapsedMs, unsigned long totalMs);
  LiquidCrystal& lcd_;
};