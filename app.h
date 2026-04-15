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

/**
  Coordinates button input, timer state, and screen updates.
*/ 
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

  HoldAction holdAction_ = HoldAction::NONE;
  unsigned long holdStartedAt_ = 0;

  /** 
    these will be used to ensure that the actual progress bar during a hold actually fills in the UI before the hold action can execute. its avoid having a janky progress bar in the UI.
    */
  // True once the hold progress bar has fully filled. 
  bool holdConfirmed_ = false;
  // Time when the hold progress bar first reached full.
  unsigned long holdConfirmedAt_ = 0;

};