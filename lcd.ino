#include <LiquidCrystal.h>
#include "button.h";

struct FormattedTime {
  int seconds;
  int minutes;
};

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

unsigned long startTime;
// const long focusTime = 1500000 // 25 minutes
unsigned long focusTime = 5000;  // 5 seconds
unsigned long breakTime = 3000;  // 3 seconds;

long remainingTime = focusTime;

bool focusMode = true;
bool modeJustEnded = false;
unsigned long modeEndedAt = 0;

unsigned long timerPausedAt;

Button resetBtn(resetBtnPin);
Button selectBtn(selectBtnPin);
Button menuBtn(menuBtnPin);
Button pauseBtn(pauseBtnPin);

enum TimerState {
  PAUSED,
  RUNNING,
};

enum Screen {
  MENU,
  TIMER,
  CONFIG,
};

TimerState timerState = RUNNING;
Screen screenState = MENU;

int selectedIndex = 0;

void setup() {
  lcd.begin(16, 2);
  lcd.clear();
  lcd.setCursor(0, 0);

  Serial.begin(9600);

  pinMode(pauseBtnPin, INPUT);
  pinMode(resetBtnPin, INPUT);
  pinMode(menuBtnPin, INPUT);
  pinMode(selectBtnPin, INPUT);
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
  if (timerState == PAUSED) {
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

void initTimer() {
  if (screenState == TIMER) {
    lcd.clear();
    lcd.setCursor(0, 0);

    startTime = millis();
    remainingTime = focusTime;
    focusMode = true;
    timerState = RUNNING;

    modeEndedAt = 0;
    modeJustEnded = false;

    timerPausedAt = 0;
  }
}

void toggleTimer() {
  if (timerState == RUNNING) {
    timerPausedAt = millis();
    timerState = PAUSED;
  } else {
    unsigned long pausedFor = millis() - timerPausedAt;

    startTime += pausedFor;

    if (modeJustEnded) {
      modeEndedAt += pausedFor;
    }

    timerState = RUNNING;
  }
}

long getRemainingTime() {
  long time;
  unsigned long elapsedTime = millis() - startTime;

  if (focusMode) {
    time = focusTime - elapsedTime;
  } else {
    time = breakTime - elapsedTime;
  }

  return time;
}

FormattedTime formatTime() {
  FormattedTime t;

  long totalSeconds = (remainingTime + 999) / 1000;
  t.seconds = totalSeconds % 60;
  t.minutes = totalSeconds / 60;

  return t;
}

void resetTime(int resetTimer) {
  if (resetTimer == HIGH) {
    Serial.println("Resetting timer...");
    startTime = millis();

    if (timerState == PAUSED) {
      timerPausedAt = startTime;
      remainingTime = getRemainingTime();
    }

    // we want to be able to reset during the transitioning phase (rendering 0:00).
    modeEndedAt = 0;
    modeJustEnded = false;
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
        screenState = TIMER;
        lcd.clear();
        initTimer();
        Serial.println("Starting Timer...");
      } else if (selectedIndex == 1) {
        lcd.clear();
        selectedIndex = 0;
        screenState = CONFIG;
        Serial.println("Rendering config...");
      }
    } else if (screenState == CONFIG) {
      if (selectedIndex == 0) {
        focusTime = 5000;
        breakTime = 3000;
        Serial.println("Selected 5 seconds focus, 3 seconds break");
      } else if (selectedIndex == 1) {
        focusTime = 10000;
        breakTime = 5000;
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
      toggleTimer();
    }

    if (resetBtn.wasPressed(millis())) {
      resetTime(HIGH);
    }

    switch (timerState) {
      case PAUSED:
        {
          auto time = formatTime();

          render(time.minutes, time.seconds, focusMode);
          break;
        }
      case RUNNING:
        {

          if (modeJustEnded) {
            // render 0:00 for 1 second before transitioning to the next mode
            if (millis() - modeEndedAt >= 1000) {
              focusMode = !focusMode;
              startTime = millis();
              remainingTime = getRemainingTime();
              modeJustEnded = false;
            }
          } else {
            remainingTime = getRemainingTime();

            if (remainingTime <= 0) {
              remainingTime = 0;
              modeEndedAt = millis();
              modeJustEnded = true;
            }
          }


          auto time = formatTime();

          render(time.minutes, time.seconds, focusMode);

          break;
        }
    }
  }
}