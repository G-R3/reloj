#pragma once

#include <Arduino.h>

class Button {
public:
  explicit Button(byte pin);

  void begin();
  // Return true once when a debounced press is detected.
  bool wasPressed(unsigned long now, unsigned long debounceMs = 50);
  // Return true once when the button has been held long enough.
  bool wasLongPressed(unsigned long now, unsigned long holdMs = 800, unsigned long debounceMs = 50);
  // Return whether the button is currently held after debouncing.
  bool isPressed(unsigned long now, unsigned long debounceMs = 50);
  // Return true once when a debounced release is detected.
  bool wasReleased(unsigned long now, unsigned long debounceMs = 50);

private:
  void update(unsigned long now, unsigned long debounceMs = 50);

  byte pin_;
  // Last stable button state after debouncing.
  bool stableState_ = 0;
  // Most recent raw reading from the pin.
  bool lastReading_ = 0;
  // Time when the raw reading last changed. used in our debounce checks
  unsigned long lastChangedTime_ = 0;

  unsigned long pressedAt_ = 0;
  // Tracks whether the current press already counted as a long press.
  unsigned long isLongPress_ = false;
  // True when a debounced press event is ready to be consumed.
  bool isPressed_ = false;
  // True when a debounced release event is ready to be consumed.
  bool wasReleased_ = false;
  // Time when the button state was last updated.
  unsigned long lastUpdatedAt_ = 0;
  // True after the first call to update().
  bool hasUpdated_ = false;
};
