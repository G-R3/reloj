#include "HardwareSerial.h"
#include "app.h"

namespace button_pins {
constexpr uint8_t pauseBtnPin = 6;
constexpr uint8_t resetBtnPin = 7;
constexpr uint8_t menuNavBtnPin = 8;
constexpr uint8_t selectBtnPin = 9;
}

App::App(LiquidCrystal &dp)
  : display_(dp),
    pauseBtn_(button_pins::pauseBtnPin),
    resetBtn_(button_pins::resetBtnPin),
    menuNavBtn_(button_pins::menuNavBtnPin),
    selectBtn_(button_pins::selectBtnPin) {}

void App::begin(unsigned long now) {
  screen_ = Screen::MENU;
  selectedIndex_ = 0;

  pauseBtn_.begin();
  resetBtn_.begin();
  menuNavBtn_.begin();
  selectBtn_.begin();

  display_.begin();
  display_.renderMenu(selectedIndex_);
}

void App::update() {
  unsigned long now = millis();

  handleMenuNav(now);
  if (screen_ != Screen::TIMER) {
    handleMenuInput(now);
  } else {
    handleTimerInput(now);
  }

  if (screen_ == Screen::MENU) {
    display_.renderMenu(selectedIndex_);
  } else if (screen_ == Screen::TIMER) {
    timer_.update(now);

    auto t = timer_.format();

    display_.renderTimer(t.minutes, t.seconds, timer_.session() == TimerSession::FOCUS, timer_.state() == TimerState::PAUSED);
  } else {
    display_.renderConfig(selectedIndex_);
  }
}


void App::handleMenuInput(unsigned long now) {
  if (selectBtn_.wasPressed(now)) {
    if (screen_ == Screen::MENU) {
      if (selectedIndex_ == 0) {
        display_.clear();
        timer_.begin(now);
        screen_ = Screen::TIMER;
        Serial.println("Starting timer...");
      } else if (selectedIndex_ == 1) {
        display_.clear();
        screen_ = Screen::CONFIG;
        Serial.println("Rendering config...");
      }
    } else if (screen_ == Screen::CONFIG) {
      if (selectedIndex_ == 0) {
        long focusTime = 5000;
        long breakTime = 3000;
        timer_.setDurations(focusTime, breakTime);
        Serial.println("Selected 5 seconds focus, 3 seconds break. Returning to menu...");
      } else if (selectedIndex_ == 1) {
        long focusTime = 10000;
        long breakTime = 5000;
        timer_.setDurations(focusTime, breakTime);
        Serial.println("Selected 10 seconds focus, 5 seconds break. Returning to menu..");
      }

      display_.clear();
      screen_ = Screen::MENU;
      selectedIndex_ = 1;
    }
  }
}

void App::handleTimerInput(unsigned long now) {
  if (selectBtn_.wasPressed(now)) {
    display_.clear();
    screen_ = Screen::MENU;
    selectedIndex_ = 0;
    Serial.println("Returning to menu...");
  } else if (pauseBtn_.wasPressed(now)) {
    timer_.togglePause(now);

    bool isPaused = timer_.state() == TimerState::PAUSED;
    const char* status = isPaused ? "Timer is paused..." : "Timer is unpaused...";
    
    Serial.println(status);
  } else if (resetBtn_.wasPressed(now)) {
    timer_.reset(now);
    Serial.println("Timer was reset...");
  }
}

void App::handleMenuNav(unsigned long now) {
  if (menuNavBtn_.wasPressed(now) && screen_ != Screen::TIMER) {
    Serial.println("Navigating menu...");
    selectedIndex_ += 1;

    if (selectedIndex_ > 1) {
      selectedIndex_ = 0;
    }
  }
}