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

struct Config {
  uint8_t magic;
  bool buzzerEnabled;
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
  void startHoldAction(HoldAction action, unsigned long now);
  void resetHoldActionState();
  void cancelHoldAction(unsigned long now);
  void executeHoldAction(unsigned long now);

  void playBuzzer();

  void loadConfig();
  void saveConfig();

  Config config_;

  Button pauseBtn_;
  Button resetBtn_;
  Button menuNavBtn_;
  Button selectBtn_;

  Timer timer_;
  Display display_;

  int selectedItemIndex_ = 0;

  Screen screen_ = Screen::MENU;

  HoldAction holdAction_ = HoldAction::NONE;
  unsigned long holdStartedAt_ = 0;

  /**
    Tracks when the hold progress bar has visually completed so the action fires
    only after the user sees the fully filled UI state.
  */
  // True once the hold progress bar has fully filled.
  bool holdConfirmed_ = false;
  // Time when the hold progress bar first reached full.
  unsigned long holdConfirmedAt_ = 0;
};