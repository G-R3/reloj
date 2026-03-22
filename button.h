class Button {
public:
  Button(byte pin) {
    pin_ = pin;
  }

  bool wasPressed(unsigned long now, unsigned long debounceMs = 50) {
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

private:
  byte pin_;
  bool stableState = LOW;
  bool lastReading = LOW;
  unsigned long lastChangedTime = 0;
};
