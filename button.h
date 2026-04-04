#pragma once

#include <Arduino.h>

class Button {
public:
  explicit Button(byte pin);

  void begin();
  bool wasPressed(unsigned long now, unsigned long debounceMs = 50);
  bool wasLongPressed(unsigned long now,unsigned long debounceMs = 50, unsigned long holdMs = 800);

private:
  byte pin_;
  bool stableState_ = 0;
  bool lastReading_ = 0;
  unsigned long lastChangedTime_ = 0;

  unsigned long pressedAt_ = 0;
  unsigned long isLongPress_ = false;
};
