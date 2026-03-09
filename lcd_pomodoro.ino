#include <LiquidCrystal.h>

const int pauseStatePin = 6;

const int rs = 12;
const int enable = 11;
const int d4 = 5;
const int d5 = 4;
const int d6 = 3;
const int d7 = 2;

LiquidCrystal lcd(rs, enable, d4, d5, d6, d7);


unsigned long start;
unsigned long passedTime;
// const long focusTime = 1500000 // 25 minutes
const long focusTime = 5000; // 5 seconds
const long breakTime = 4000; // 4 seconds;

long remainingTime;

bool focusMode = true;

int pauseState = 0;
int prevPauseState = 0;
int timerEnabled = 1;


unsigned long timeAtPaused;

void setup() {
  lcd.begin(16, 2);

  start = millis();

  Serial.begin(9600);
}


void render(int minutes, int seconds, bool focusMode) {
  lcd.setCursor(0,0);

  if(focusMode) {
    lcd.print("Focus Time");
  } else {
    lcd.print("Break Time");
  }

  lcd.setCursor(0,1);
  lcd.print(minutes);
  lcd.print(":");

  if (seconds < 10) lcd.print("0");

  lcd.print(seconds);


  lcd.setCursor(5, 1);
  if (!timerEnabled) {
    lcd.print("PAUSED");
  } else {
    lcd.print("      ");
  }
  
}


long getRemainingTime(bool focusMode) {
  long remainingTime;

  if(focusMode) {
    remainingTime = focusTime - (millis() - start);
  } else {
    remainingTime = breakTime - (millis() - start);
  }

  return remainingTime;
}


void loop() {
  pauseState = digitalRead(pauseStatePin);
  Serial.println(pauseState);

  if(pauseState == 1 && prevPauseState == 0) {
    timeAtPaused = millis();
    timerEnabled = !timerEnabled;
  } 



  if(timerEnabled) {

    remainingTime = getRemainingTime(focusMode);
    
    if(remainingTime <= 0) {
      remainingTime = 0;
      start = millis();
      focusMode = !focusMode;
    }
  }






  long totalSeconds = (remainingTime) / 1000;
  int seconds = totalSeconds % 60;
  int minutes = totalSeconds / 60;
  render(minutes, seconds, focusMode);

  prevPauseState = pauseState;
 
}




/**
  LCD pomodoro
  - Menu
    - start
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


*/