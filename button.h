#pragma once

#include <Arduino.h>

class Button {
public:
  explicit Button(byte pin);

  void begin();
  bool wasPressed(unsigned long now, unsigned long debounceMs = 50);
  bool wasLongPressed(unsigned long now,unsigned long debounceMs = 50, unsigned long holdMs = 800);
  /** whether the button is currently being held after debouncing */
  bool isPressed(unsigned long now, unsigned long debounceMs = 50);
  /** whether the button was released after deboucing */
  bool wasReleased(unsigned long now, unsigned long debounceMs = 50);

private:
  void update(unsigned long now, unsigned long debounceMs = 50);

  byte pin_;
  bool stableState_ = 0;
  bool lastReading_ = 0;
  unsigned long lastChangedTime_ = 0;

  unsigned long pressedAt_ = 0;
  unsigned long isLongPress_ = false;
  // whether the push button was pressed after debounce
  bool isPressed_ = false; 
  bool wasReleased_ = false;
  unsigned long lastUpdatedAt_ = 0;
  bool hasUpdated_ = false;
};
