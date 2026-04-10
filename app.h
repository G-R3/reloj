#pragma once

#include "button.h"
#include "timer.h"
#include "display.h"

enum class Screen { MENU,
                    TIMER,
                    CONFIG };

enum class AppFronzenTimerIntent {
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
  void handleFronzeTimer(unsigned long now);
  void cancelFronzenTimer(unsigned long now);
  void executeFrozenIntent(unsigned long now);

  Button pauseBtn_;
  Button resetBtn_;
  Button menuNavBtn_;
  Button selectBtn_;

  Timer timer_;
  Display display_;

  int selectedIndex_ = 0;

  Screen screen_ = Screen::MENU;
  AppFronzenTimerIntent fronzeTimerIntent_ = AppFronzenTimerIntent::NONE;
  unsigned long timerFreezeStartedAt_ = 0;
};