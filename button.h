class Button {
    public:
        Button (byte pin);
    bool wasPressed(DebouncedButton &btn, unsigned long now, unsigned long debounceMs = 50) {
        bool reading = digitalRead(btn.pin);
      
        if (reading != btn.lastReading) {
          btn.lastReading = reading;
          btn.lastChangedTime = now;
        }
      
        if ((now - btn.lastChangedTime) >= debounceMs && reading != btn.stableState) {
          btn.stableState = reading;
          if (btn.stableState == HIGH) {
            return true;
          }
        }
      
      
        return false;
      }
}