#include <Arduino.h>

class Button {
public:
  explicit Button(byte pin);

  void begin();
  bool wasPressed(unsigned long now, unsigned long debounceMs = 50);

private:
  byte pin_;
  bool stableState = 0;
  bool lastReading = 0;
  unsigned long lastChangedTime = 0;
};
