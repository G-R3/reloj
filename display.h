#include <LiquidCrystal.h>

class Display {
public:
  Display(LiquidCrystal& dp);
  void begin();
  void renderTimer(int minutes, int seconds, bool isFocused, bool isPaused);
  void renderMenu(int selectedIndex);
  void renderConfig(int selectedIndex);
  void clear();

  private:
    LiquidCrystal lcd;
};