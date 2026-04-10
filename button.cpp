#include "button.h"

Button::Button(byte pin)
  : pin_(pin) {}

void Button::begin() {
  pinMode(pin_, INPUT);
}

void Button::update(unsigned long now, unsigned long debounceMs) {
  if (hasUpdated_ && lastUpdatedAt_ == now) {
    return;
  }

  hasUpdated_ = true;
  lastUpdatedAt_ = now;
  isPressed_ = false;
  wasReleased_ = false;

  bool reading = digitalRead(pin_);

  if (reading != lastReading_) {
    lastReading_ = reading;
    lastChangedTime_ = now;
  }

  if ((now - lastChangedTime_) >= debounceMs && reading != stableState_) {
    stableState_ = reading;

    if (stableState_ == HIGH) {
      pressedAt_ = now;
      isPressed_ = true;
      isLongPress_ = false;
    } else {
      wasReleased_ = true;
      pressedAt_ = 0;
      isLongPress_ = false;
    }
  }
}

bool Button::wasPressed(unsigned long now, unsigned long debounceMs) {
  update(now, debounceMs);

  return isPressed_;
}

bool Button::wasLongPressed(unsigned long now, unsigned long holdMs, unsigned long debounceMs) {
  update(now, debounceMs);

  if (stableState_ == HIGH && !isLongPress_ && (now - pressedAt_) >= holdMs) {
    Serial.println("button was held past hold threshold...");
    isLongPress_ = true;
    return true;
  }

  Serial.println("NOT held...");

  return false;
}

bool Button::isPressed(unsigned long now, unsigned long debounceMs) {
  update(now, debounceMs);

  const char* status = stableState_ == HIGH ? "Button is being held..." : "Button is not pressed...";
  Serial.println(status);
  return stableState_ == HIGH;
}

bool Button::wasReleased(unsigned long now, unsigned long debounceMs) {
  update(now, debounceMs);

  const char* status = wasReleased_ ? "Button was released..." : "Button is not released...";
  Serial.println(status);
  return wasReleased_;
}