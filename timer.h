#pragma once

enum class TimerState { PAUSED,
                        RUNNING };
enum class TimerSession { FOCUS,
                          BREAK };

struct FormattedTime {
  int minutes;
  int seconds;
};

class Timer {
public:
  void begin(unsigned long now);
  void update(unsigned long now);
  void togglePause(unsigned long now);
  void reset(unsigned long now);
  void setDurations(unsigned long focusMs, unsigned long breakMs);
  void skip(unsigned long now);
  void beginTimerFreeze(unsigned long now);
  void endTimerFreeze(unsigned long now, bool compensateElapsed = false); 
  bool isTimerFrozen() const;


  long remainingMs() const;
  FormattedTime format() const;
  TimerState state() const;
  TimerSession session() const;

private:
  long computeRemainingMs(unsigned long now) const;
  unsigned long startMs_ = 0;
  unsigned long focusMs_ = 5000;
  unsigned long breakMs_ = 3000;
  long remainingMs_ = focusMs_;

  TimerState state_ = TimerState::RUNNING;
  TimerSession session_ = TimerSession::FOCUS;

  bool modeJustEnded_ = false;
  unsigned long modeEndedAt_ = 0;

  unsigned long pausedAt_ = 0;
  // whether the timer progression is tempirarily frozen
  unsigned long timerFronzen_ = false;
  // whent he timer was frozen
  unsigned long timerFronzenAt_ = 0;
  // whether the frozen time should be added back into the timer when the frezze is canceled
  unsigned long timerFronzeCompensatesTime_ = false;
};
