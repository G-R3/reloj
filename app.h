#pragma once

#include "button.h"
#include "timer.h"
#include "display.h"

enum class Screen { MENU,
                    TIMER,
                    CONFIG };

enum class HoldAction {
  NONE,
  BACK_TO_MENU,
  SKIP_TIMER_SESSION
};

class App {
public:
  explicit App(LiquidCrystal& lcd);
  void begin(unsigned long now);
  void update();

private:
  void handleMenuSelect(unsigned long now);
  void handleTimerInput(unsigned long now);
  void handleMenuNav(unsigned long now);
  
  void handleHoldAction(unsigned long now);
  void cancelHoldAction(unsigned long now);
  void executeHoldAction(unsigned long now);

  Button pauseBtn_;
  Button resetBtn_;
  Button menuNavBtn_;
  Button selectBtn_;

  Timer timer_;
  Display display_;

  int selectedIndex_ = 0;

  Screen screen_ = Screen::MENU;
  HoldAction holdAction = HoldAction::NONE;
  unsigned long holdStartedAt_ = 0;
};