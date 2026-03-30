#include "app.h";

App::App(LiquidCrystal &dp) : display_(dp) {}

void App::begin(unsigned long now) {
  screen_ = Screen::MENU;
  selectedIndex_ = 0;

  timer_.begin(now);
  pauseBtn_.begin();
  resetBtn_.begin();
  selectBtn_.begin();
  menuBtn_.begin();


  display_.begin();
  // display_.renderMenu(selectedIndex_);
}

void App::update() {
  handleSelect();

  if (screen_ == Screen::MENU) {
  }
}


void App::handleMenuInput(unsigned long now) {
}

void App::handleTimerInput(unsigned long now) {
}

void App::handleSelect() {
  if (menuBtn_.wasPressed(millis()) && screen_ != Screen::TIMER) {
    Serial.println("IN APP Navigating menu...");
    selectedIndex_ += 1;

    if (selectedIndex_ > 1) {
      selectedIndex_ = 0;
    }
  }
}