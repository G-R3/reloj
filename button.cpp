#include "button.h"

Button::Button(byte pin)
  : pin_(pin) {}

void Button::begin() {
  pinMode(pin_, INPUT);
  const bool initialReading = digitalRead(pin_) == HIGH;
  stableState_ = initialReading;
  lastReading_ = initialReading;
  lastChangedTime_ = millis();
  pressedAt_ = 0;
  longPressConsumed_ = false;
  isPressed_ = false;
  wasReleased_ = false;
  hasUpdated_ = false;
  lastUpdatedAt_ = 0;
}

void Button::update(unsigned long now, unsigned long debounceMs) {
  if (hasUpdated_ && lastUpdatedAt_ == now) {
    return;
  }

  hasUpdated_ = true;
  lastUpdatedAt_ = now;
  isPressed_ = false;
  wasReleased_ = false;

  const bool reading = digitalRead(pin_) == HIGH;

  if (reading != lastReading_) {
    lastReading_ = reading;
    lastChangedTime_ = now;
  }

  if ((now - lastChangedTime_) >= debounceMs && reading != stableState_) {
    stableState_ = reading;

    if (stableState_ == HIGH) {
      pressedAt_ = now;
      isPressed_ = true;
      longPressConsumed_ = false;
    } else {
      wasReleased_ = true;
      pressedAt_ = 0;
      longPressConsumed_ = false;
    }
  }
}

bool Button::wasPressed(unsigned long now, unsigned long debounceMs) {
  update(now, debounceMs);

  return isPressed_;
}

bool Button::wasLongPressed(unsigned long now, unsigned long holdMs, unsigned long debounceMs) {
  update(now, debounceMs);

  if (stableState_ == HIGH && !longPressConsumed_ && (now - pressedAt_) >= holdMs) {
    longPressConsumed_ = true;
    return true;
  }

  return false;
}

bool Button::isPressed(unsigned long now, unsigned long debounceMs) {
  update(now, debounceMs);
  return stableState_ == HIGH;
}

bool Button::wasReleased(unsigned long now, unsigned long debounceMs) {
  update(now, debounceMs);
  return wasReleased_;
}