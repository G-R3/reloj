#include "button.h"

Button::Button(byte pin) : pin_(pin) {}

void Button::begin() {
  pinMode(pin_, INPUT);
}

bool Button::wasPressed(unsigned long now, unsigned long debounceMs) {
  bool reading = digitalRead(pin_);

  if (reading != lastReading) {
    lastReading = reading;
    lastChangedTime = now;
  }

  if ((now - lastChangedTime) >= debounceMs && reading != stableState) {
    stableState = reading;
    if (stableState == HIGH) {
      return true;
    }
  }

  return false;
}
