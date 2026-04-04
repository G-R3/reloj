#include "button.h"

Button::Button(byte pin)
  : pin_(pin) {}

void Button::begin() {
  pinMode(pin_, INPUT);
}

void Button::update(unsigned long now, unsigned long debounceMs) {
  bool reading = digitalRead(pin_);

  if (reading != lastReading_) {
    lastReading_ = reading;
    lastChangedTime_ = now;
  }

  if ((now - lastChangedTime_) >= debounceMs && reading != stableState_) {
    stableState_ = reading;

    if (stableState_ == HIGH) {
      pressedAt_ = now;
      longPressHandled_ = false;
    } else {
      pressedAt_ = 0;
      longPressHandled_ = false;
    }
  }
}

bool Button::wasPressed(unsigned long now, unsigned long debounceMs) {
  update(now, debounceMs);

  // isn't pressedAT_ ONLY ever going to be equal to now when stableState_ == HIGH?
  // if so, stableState_ is not necessary?
  if (stableState_ == HIGH && pressedAt_ == now) {
    return true;
  }

  return false;
}

bool Button::wasLongPressed(unsigned long now, unsigned long holdMs, unsigned long debounceMs) {
  update(now, debounceMs);

    // if we have clicked the button, have passed the hold threshold, and i have yet to
  // mark this as a long press... mark it as a long press.
  if (stableState_ == HIGH && !longPressHandled_ && (now - pressedAt_) >= holdMs) {
    longPressHandled_ = true;
    return true;
  }

  return false;
}