#include <LiquidCrystal.h>

struct FormattedTime {
  int seconds;
  int minutes;
};

const int pauseBtnPin = 6;

const int rs = 12;
const int enable = 11;
const int d4 = 5;
const int d5 = 4;
const int d6 = 3;
const int d7 = 2;

LiquidCrystal lcd(rs, enable, d4, d5, d6, d7);

unsigned long startTime;
unsigned long passedTime;
// const long focusTime = 1500000 // 25 minutes
const long focusTime = 10000;  // 10 seconds
const long breakTime = 8000;   // 8 seconds;

long remainingTime;

bool focusMode = true;

int currentPauseState = 0;
int prevPauseState = 0;

unsigned long timerPausedAt;

enum TimerState {
  PAUSED,
  RUNNING,
};

TimerState timerState = RUNNING;

void setup() {
  lcd.begin(16, 2);
  lcd.clear();
  lcd.setCursor(0, 0);

  startTime = millis();

  Serial.begin(9600);
  pinMode(pauseBtnPin, INPUT);
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
  long remainingTime;
  unsigned long elapsedTime = millis() - startTime;

  if (focusMode) {
    remainingTime = focusTime - elapsedTime;
  } else {
    remainingTime = breakTime - elapsedTime;
  }

  return remainingTime;
}

FormattedTime formatTime() {
  FormattedTime t;

  long totalSeconds = (remainingTime) / 1000;
  t.seconds = totalSeconds % 60;
  t.minutes = totalSeconds / 60;

  return t;
}

void loop() {
  currentPauseState = digitalRead(pauseBtnPin);
  Serial.println(currentPauseState);
  toggleTimer(currentPauseState);

  switch (timerState) {
    case PAUSED:
      {
        auto time = formatTime();

        render(time.minutes, time.seconds, focusMode);
        break;
      }
    case RUNNING:
      {
        remainingTime = getRemainingTime();

        if (remainingTime <= 0) {
          remainingTime = 0;
          startTime = millis();
          focusMode = !focusMode;
        }

        auto time = formatTime();

        render(time.minutes, time.seconds, focusMode);
        break;
      }
  }

  prevPauseState = currentPauseState;
}


/**
  LCD pomodoro
  - Menu
    - startTime
    - configure
      - Set focus duration
      - Set break duration
        - maybe have some checks so that !(break > focus)
    stats
      - ???
    

    menu navigation:
    - potentiometer, rotate to go through menu items. push button to confirm menu time
    - push buttons. 1 push button to up a menu item and another to go down.

    timer screen
    - push button to pause
    - push button to reset timer
    - push button to exit to menu.


    Time states:
      PAUSED:
        - timer does not count down
        - record the time at the moment of pausing
      RESUME:
        - timer updates startTime. startTime + (millis() - timePausedAt)
          - millis() does not stop counting when we pause.
      RUNNING:
        - timer counts down
        - do no recalculate startTime.

*/