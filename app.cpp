#include "HardwareSerial.h"
#include "app.h"

App::App(LiquidCrystal &dp)
  : display_(dp) {}

void App::begin(unsigned long now) {
  screen_ = Screen::MENU;
  selectedIndex_ = 0;

  timer_.begin(now);
  pauseBtn_.begin();
  resetBtn_.begin();
  menuBtn_.begin();
  menuBtn_.begin();

  display_.begin();
  display_.renderMenu(selectedIndex_);
}

void App::update() {
  unsigned long now = millis();
  
  handleSelect(now);
  if(screen_ != Screen::TIMER) {
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
  if (menuBtn_.wasPressed(now)) {
    if (screen_ == Screen::MENU) {
      if (selectedIndex_ == 0) {
        display_.clear();
        timer_.begin(now);
        screen_ = Screen::TIMER;
      } else if (selectedIndex_ == 1) {
        display_.clear();
        screen_ = Screen::CONFIG;
      }
    } else if (screen_ == Screen::CONFIG) {
      if (selectedIndex_ == 0) {
        long focusTime = 5000;
        long breakTime = 3000;
        timer_.setDurations(focusTime, breakTime);
        Serial.println("Selected 5 seconds focus, 3 seconds break");
      } else if (selectedIndex_ == 1) {
        long focusTime = 10000;
        long breakTime = 5000;
        timer_.setDurations(focusTime, breakTime);
        Serial.println("Selected 10 seconds focus, 5 seconds break");
      }

      display_.clear();
      screen_ = Screen::MENU;
      selectedIndex_ = 1;
    }
  }
}

void App::handleTimerInput(unsigned long now) {
  if (menuBtn_.wasPressed(now)) { 
    display_.clear();
    screen_ = Screen::MENU;
    selectedIndex_ = 0;
  } else if(pauseBtn_.wasPressed(now)) {
    timer_.togglePause(now);
  } else if(resetBtn_.wasPressed(now)) {
    timer_.reset(now);
  }
}

void App::handleSelect(unsigned long now) {
  if (selectBtn_.wasPressed(now) && screen_ != Screen::TIMER) {
    Serial.println("IN APP Navigating menu...");
    selectedIndex_ += 1;

    if (selectedIndex_ > 1) {
      selectedIndex_ = 0;
    }
  } 
}