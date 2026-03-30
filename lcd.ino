#include <LiquidCrystal.h>
#include "button.h";
#include "timer.h";

Timer timer;

const int pauseBtnPin = 6;
const int resetBtnPin = 7;
const int menuBtnPin = 8;
const int selectBtnPin = 9;

const int rs = 12;
const int enable = 11;
const int d4 = 5;
const int d5 = 4;
const int d6 = 3;
const int d7 = 2;

LiquidCrystal lcd(rs, enable, d4, d5, d6, d7);

Button resetBtn(resetBtnPin);
Button selectBtn(selectBtnPin);
Button menuBtn(menuBtnPin);
Button pauseBtn(pauseBtnPin);

enum Screen {
  MENU,
  TIMER,
  CONFIG,
};

Screen screenState = MENU;

int selectedIndex = 0;

void setup() {
  lcd.begin(16, 2);
  lcd.clear();
  lcd.setCursor(0, 0);

  Serial.begin(9600);

  pauseBtn.begin();
  resetBtn.begin();
  menuBtn.begin();
  selectBtn.begin();
}

void render(int minutes, int seconds, bool focusMode) {
  lcd.setCursor(0, 0);

  if (focusMode) {
    lcd.print("Focus");
  } else {
    lcd.print("Break");
  }

  lcd.setCursor(0, 1);
  if (minutes < 10) lcd.print("0");
  lcd.print(minutes);
  lcd.print(":");
  if (seconds < 10) lcd.print("0");
  lcd.print(seconds);
  lcd.print(" ");


  lcd.setCursor(5, 1);
  if (timer.state() == TimerState::PAUSED) {
    lcd.print(" PAUSED");
  } else {
    lcd.print("       ");
  }
}

void renderMenu() {
  if (screenState == MENU) {
    lcd.setCursor(0, 0);
    if (selectedIndex == 0) {
      lcd.print(">");
    } else {
      lcd.print(" ");
    }
    lcd.print("Start");

    lcd.setCursor(0, 1);
    if (selectedIndex == 1) {
      lcd.print(">");
    } else {
      lcd.print(" ");
    }
    lcd.print("Config");
  }
}

void renderConfig() {
  if (screenState == CONFIG) {
    lcd.setCursor(0, 0);
    if (selectedIndex == 0) {
      lcd.print(">");
    } else {
      lcd.print(" ");
    }
    lcd.print("5/3");

    lcd.setCursor(0, 1);
    if (selectedIndex == 1) {
      lcd.print(">");
    } else {
      lcd.print(" ");
    }
    lcd.print("10/5");
  }
}


void loop() {
  if (menuBtn.wasPressed(millis()) && screenState != TIMER) {
    Serial.println("Navigating menu...");
    selectedIndex += 1;

    if (selectedIndex > 1) {
      selectedIndex = 0;
    }
  }
  if (selectBtn.wasPressed(millis())) {
    if (screenState == TIMER) {
      lcd.clear();
      screenState = MENU;
      Serial.println("Returning to menu...");
      // renderMenu();
    } else if (screenState == MENU) {
      if (selectedIndex == 0) {
        lcd.clear();
        lcd.setCursor(0, 0);
        timer.begin(millis());
        screenState = TIMER;
        Serial.println("Starting Timer...");
      } else if (selectedIndex == 1) {
        lcd.clear();
        selectedIndex = 0;
        screenState = CONFIG;
        Serial.println("Rendering config...");
      }
    } else if (screenState == CONFIG) {
      if (selectedIndex == 0) {
        long focusTime = 5000;
        long breakTime = 3000;
        timer.setDurations(focusTime, breakTime);
        Serial.println("Selected 5 seconds focus, 3 seconds break");
      } else if (selectedIndex == 1) {
        long focusTime = 10000;
        long breakTime = 5000;
        timer.setDurations(focusTime, breakTime);
        Serial.println("Selected 10 seconds focus, 5 seconds break");
      }

      screenState = MENU;
      selectedIndex = 1;
    }
  }

  if (screenState == MENU) {
    renderMenu();
  } else if (screenState == CONFIG) {
    renderConfig();
  } else if (screenState == TIMER) {
    if (pauseBtn.wasPressed(millis())) {
      timer.togglePause(millis());
    }

    if (resetBtn.wasPressed(millis())) {
      timer.reset(millis());
    }

    switch (timer.state()) {
      case TimerState::PAUSED:
        {
          auto t = timer.format();

          render(t.minutes, t.seconds, timer.session() == TimerSession::FOCUS);
          break;
        }
      case TimerState::RUNNING:
        {
          timer.update(millis());

          auto t = timer.format();

          render(t.minutes, t.seconds, timer.session() == TimerSession::FOCUS);

          break;
        }
    }
  }
}

// #include "LiquidCrystal.h"
// #include "app.h"

// const int rs = 12;
// const int enable = 11;
// const int d4 = 5;
// const int d5 = 4;
// const int d6 = 3;
// const int d7 = 2;


// LiquidCrystal lcd(rs, enable, d4, d5, d6, d7);
// App app(lcd);

// void setup() {
//   app.begin(millis());
// }

// void loop() {
//   app.update();
// }