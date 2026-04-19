#include "HardwareSerial.h"
#include "Arduino.h"
#include <EEPROM.h>
#include "app.h"

namespace pins {
constexpr uint8_t pauseBtnPin = 6;
constexpr uint8_t resetBtnPin = 7;
constexpr uint8_t menuNavBtnPin = 8;
constexpr uint8_t selectBtnPin = 9;
constexpr uint8_t piezoPin = 10;
}

namespace menu_items {
constexpr int start = 0;
constexpr int config = 1;
constexpr int count = 2;
}

namespace config_items {
constexpr int shortPreset = 0;
constexpr int longPreset = 1;
constexpr int buzzer = 2;
constexpr int count = 3;
}

namespace preset_durations {
constexpr unsigned long shortFocusMs = 5000UL;
constexpr unsigned long shortBreakMs = 3000UL;
constexpr unsigned long longFocusMs = 10000UL;
constexpr unsigned long longBreakMs = 5000UL;
}

namespace buzzer_config {
constexpr unsigned int toneHz = 2200;
constexpr unsigned long toneDurationMs = 180;
}

namespace button_timing {
constexpr unsigned long executeHoldMs = 800;
constexpr unsigned long holdFlashMs = 75;
}

namespace storage_config {
constexpr uint8_t magic = 0x42;
constexpr int configAddr = 0;
}

App::App(LiquidCrystal& dp)
  : display_(dp),
    pauseBtn_(pins::pauseBtnPin),
    resetBtn_(pins::resetBtnPin),
    menuNavBtn_(pins::menuNavBtnPin),
    selectBtn_(pins::selectBtnPin) {}

void App::begin(unsigned long now) {
  (void)now;
  pauseBtn_.begin();
  resetBtn_.begin();
  menuNavBtn_.begin();
  selectBtn_.begin();

  loadConfig();

  pinMode(pins::piezoPin, OUTPUT);

  display_.begin();
}

void App::loadConfig() {
  EEPROM.get(storage_config::configAddr, config_);

  // NOTE: if new fields are added to the Config, initialize them and trigger a save (click whatever buttons we need to),
  // or add versioning check here in addition to the magic check. Otherwise old EEPROM data may pass the magic check
  // while leaving new fields uninitialized.
  if (config_.magic != storage_config::magic) {
    config_.buzzerEnabled = true;
    config_.magic = storage_config::magic;

    EEPROM.put(storage_config::configAddr, config_);
  }
}

void App::saveConfig() {
  EEPROM.put(storage_config::configAddr, config_);
}

void App::update() {
  const unsigned long now = millis();

  if (screen_ != Screen::TIMER) {
    handleMenuNav(now);
    handleMenuSelect(now);
  } else {
    handleTimerInput(now);
  }

  if (screen_ == Screen::MENU) {
    display_.renderMenu(selectedIndex_);
  } else if (screen_ == Screen::TIMER) {
    timer_.update(now);

    if (holdAction_ != HoldAction::NONE) {
      const unsigned long elapsed = holdConfirmed_ ? button_timing::executeHoldMs : (now - holdStartedAt_);
      const char* holdLabel = holdAction_ == HoldAction::BACK_TO_MENU ? "Hold for menu" : "Hold to skip";
      display_.renderHold(holdLabel, elapsed, button_timing::executeHoldMs, holdConfirmed_);
    } else {
      playBuzzer();
      const auto t = timer_.format();
      display_.renderTimer(t.minutes,
                           t.seconds,
                           timer_.session() == TimerSession::FOCUS,
                           timer_.state() == TimerState::PAUSED,
                           timer_.remainingMs(),
                           timer_.sessionDurationMs());
    }
  } else {
    display_.renderConfig(selectedIndex_, config_.buzzerEnabled);
  }
}

void App::playBuzzer() {
  if (!timer_.hasSessionEnded() || !config_.buzzerEnabled) return;

  tone(pins::piezoPin, buzzer_config::toneHz, buzzer_config::toneDurationMs);
  // for (int i = 0; i < 3; i++) {
  //   tone(pins::piezoPin, 1000);
  //   delay(100);

  //   noTone(pins::piezoPin);
  //   delay(100);
  // }
}

void App::handleMenuSelect(unsigned long now) {
  if (selectBtn_.wasPressed(now)) {
    if (screen_ == Screen::MENU) {
      if (selectedIndex_ == menu_items::start) {
        timer_.begin(now);
        screen_ = Screen::TIMER;
        Serial.println("Starting timer...");
      } else if (selectedIndex_ == menu_items::config) {
        selectedIndex_ = 0;
        screen_ = Screen::CONFIG;
        Serial.println("Rendering config...");
      }
    } else if (screen_ == Screen::CONFIG) {
      if (selectedIndex_ == config_items::shortPreset) {
        timer_.setDurations(preset_durations::shortFocusMs, preset_durations::shortBreakMs);
        Serial.println("Selected 5 seconds focus, 3 seconds break. Returning to menu...");
      } else if (selectedIndex_ == config_items::longPreset) {
        timer_.setDurations(preset_durations::longFocusMs, preset_durations::longBreakMs);
        Serial.println("Selected 10 seconds focus, 5 seconds break. Returning to menu...");
      } else if (selectedIndex_ == config_items::buzzer) {
        config_.buzzerEnabled = !config_.buzzerEnabled;
        saveConfig();
        Serial.println("Buzzer toggled. Returning to menu...");
      }

      screen_ = Screen::MENU;
      selectedIndex_ = menu_items::config;
    }
  }
}

void App::startHoldAction(HoldAction action, unsigned long now) {
  timer_.beginTimerFreeze(now);
  holdAction_ = action;
  holdStartedAt_ = now;
  holdConfirmed_ = false;
  holdConfirmedAt_ = 0;
}

void App::resetHoldActionState() {
  holdAction_ = HoldAction::NONE;
  holdStartedAt_ = 0;
  holdConfirmed_ = false;
  holdConfirmedAt_ = 0;
}

void App::cancelHoldAction(unsigned long now) {
  timer_.endTimerFreeze(now, true);
  resetHoldActionState();
}

void App::executeHoldAction(unsigned long now) {
  if (holdAction_ == HoldAction::BACK_TO_MENU) {
    screen_ = Screen::MENU;
    selectedIndex_ = 0;
    timer_.endTimerFreeze(now);
    Serial.println("Returning to menu...");
  } else if (holdAction_ == HoldAction::SKIP_TIMER_SESSION) {
    timer_.endTimerFreeze(now);
    timer_.skip(now);
  }

  resetHoldActionState();
}

void App::handleHoldAction(unsigned long now) {
  Button* holdButton = nullptr;

  if (holdAction_ == HoldAction::BACK_TO_MENU) {
    holdButton = &selectBtn_;
  } else if (holdAction_ == HoldAction::SKIP_TIMER_SESSION) {
    holdButton = &menuNavBtn_;
  } else {
    return;
  }

  const bool wasLongPressed = holdButton->wasLongPressed(now, button_timing::executeHoldMs);
  const bool wasReleased = holdButton->wasReleased(now);

  if (!holdConfirmed_ && wasLongPressed) {
    holdConfirmed_ = true;
    holdConfirmedAt_ = now;
    return;
  }

  // This is a defensive fallback. If release is processed before the long-press event fires,
  // we still confirm if the button was held long enough overall.
  if (!holdConfirmed_ && wasReleased) {
    if ((now - holdStartedAt_) >= button_timing::executeHoldMs) {
      holdConfirmed_ = true;
      holdConfirmedAt_ = now;
      return;
    } else {
      cancelHoldAction(now);
      return;
    }
  }

  if (holdConfirmed_ && (now - holdConfirmedAt_) >= button_timing::holdFlashMs) {
    executeHoldAction(now);
  }
}

void App::handleTimerInput(unsigned long now) {
  if (holdAction_ != HoldAction::NONE) {
    handleHoldAction(now);
    return;
  }

  if (selectBtn_.wasPressed(now)) {
    startHoldAction(HoldAction::BACK_TO_MENU, now);
  } else if (pauseBtn_.wasPressed(now)) {
    timer_.togglePause(now);

    const bool isPaused = timer_.state() == TimerState::PAUSED;
    const char* status = isPaused ? "Timer is paused..." : "Timer is unpaused...";

    Serial.println(status);
  } else if (resetBtn_.wasPressed(now)) {
    timer_.reset(now);
    Serial.println("Timer was reset...");
  } else if (menuNavBtn_.wasPressed(now)) {
    startHoldAction(HoldAction::SKIP_TIMER_SESSION, now);
  }
}

void App::handleMenuNav(unsigned long now) {
  if (menuNavBtn_.wasPressed(now)) {
    Serial.println("Navigating menu...");
    const int itemCount = screen_ == Screen::MENU ? menu_items::count : config_items::count;
    selectedIndex_ = (selectedIndex_ + 1) % itemCount;
  }
}