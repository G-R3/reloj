#include <LiquidCrystal.h>

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

int currentPauseState = 0;
int prevPauseState = 0;
unsigned long timerPausedAt;

int resetTimer = 0;

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
    lcd.print("Focus Time");
  } else {
    lcd.print("Break Time");
  }

  lcd.setCursor(0, 1);
  lcd.print(minutes);
  lcd.print(":");

  if (seconds < 10) lcd.print("0");

  lcd.print(seconds);


  lcd.setCursor(5, 1);
  if (timerState == PAUSED) {
    lcd.print("PAUSED");
  } else {
    lcd.print("      ");
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
    focusMode = true;
  }
}

void toggleTimer(int currentPauseState) {
  if (currentPauseState == 1 && prevPauseState == 0) {
    if (timerState == RUNNING) {
      timerPausedAt = millis();
      timerState = PAUSED;
    } else {
      startTime += millis() - timerPausedAt;
      timerState = RUNNING;
    }
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
    startTime = millis();

    // we want to be able to reset during the transitioning phase (rendering 0:00).
    modeEndedAt = 0;
    modeJustEnded = false;
  }
}

void loop() {
  int navigate = digitalRead(menuBtnPin);
  if (navigate) {
    // TODO: find how to debounce button presses. this is a hacky solution
    delay(100);
    navigate = digitalRead(menuBtnPin);
    if (navigate) {
      selectedIndex += 1;

      if (selectedIndex > 1) {
        selectedIndex = 0;
      }
    }
  }
  int select = digitalRead(selectBtnPin);
  if (select) {
    // TODO: find how to debounce button presses. this is a hacky solution
    delay(50);
    select = digitalRead(selectBtnPin);
    if (select) {
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
  }

  if (screenState == MENU) {
    renderMenu();
  } else if (screenState == CONFIG) {
    renderConfig();
  } else if (screenState == TIMER) {
    currentPauseState = digitalRead(pauseBtnPin);
    toggleTimer(currentPauseState);

    resetTimer = digitalRead(resetBtnPin);
    resetTime(resetTimer);

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

    prevPauseState = currentPauseState;
  }
}