#include "button.h"
#include "timer.h"
#include "display.h"

enum class Screen { MENU,
                    TIMER,
                    CONFIG };
// const int pauseBtnPin = 6;
// const int resetBtnPin = 7;
// const int menuBtnPin = 8;
// const int selectBtnPin = 9;


class App {
public:
  explicit App(LiquidCrystal& lcd);
  void begin(unsigned long now);
  void update();

private:
  void handleMenuInput(unsigned long now);
  void handleTimerInput(unsigned long now);
  void handleMenuNav(unsigned long now);

  Button pauseBtn_{ 6 };
  Button resetBtn_{ 7 };
  Button menuNavBtn_{ 8 };
  Button selectBtn_{ 9 };

  Timer timer_;
  Display display_;
  // Button button_;

  int selectedIndex_ = 0;

  Screen screen_ = Screen::MENU;
};