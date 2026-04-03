#include "button.h"

Button::Button(byte pin)
  : pin_(pin) {}

void Button::begin() {
  pinMode(pin_, INPUT);
}

bool Button::wasPressed(unsigned long now, unsigned long debounceMs) {
  int reading = digitalRead(pin_);

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


bool Button::wasLongPress(unsigned long now, unsigned long debounceMs, unsigned long holdMs) {
  int reading = digitalRead(pin_);

  if (reading != lastReading_) {
    lastReading_ = reading;
    lastChangedTime_ = now;
  }

  if ((now - lastChangedTime_) >= debounceMs && reading != stableState_) {
    stableState_ = reading;

    if (stableState_ == HIGH) {
      pressedAt_ = now;
      isLongPress = false;
    } else {
      pressedAt_ = 0;
      isLongPress = false;
    }
  }

  // if we have clicked the button, have passed the hold threshold, and i have yet to
  // mark this as a long press... mark it as a long press.
  if(stableState_ == HIGH && !isLongPress && (now - pressedAt_) > holdMs) {
    isLongPress = true;
    return true;
  }

  return false;
}