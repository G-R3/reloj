#include "button.h"

Button::Button(byte pin)
  : pin_(pin) {}

void Button::begin() {
  pinMode(pin_, INPUT);
}

bool Button::wasPressed(unsigned long now, unsigned long debounceMs) {
  bool reading = digitalRead(pin_);

  if (reading != lastReading_) {
    lastReading_ = reading;
    lastChangedTime_ = now;
  }

  if ((now - lastChangedTime_) >= debounceMs && reading != stableState_) {
    stableState_ = reading;
    if (stableState_ == HIGH) {
      return true;
    }
  }

  return false;
}
