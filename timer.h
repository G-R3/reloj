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

  // Temporarily freeze countdown updates during a hold action.
  // This is separate from togglePause() because a hold is not a real user pause:
  // we want time to stop briefly without changing the timer state to PAUSED or
  // accidentally unpausing a timer that was already paused before the hold began.
  void beginTimerFreeze(unsigned long now);
  // End a freeze and optionally add the frozen time back.
  // Unlike togglePause(), ending a freeze restores normal updates without acting like a user pause/resume.
  void endTimerFreeze(unsigned long now, bool compensateElapsed = false); 
  // Return whether a temporary freeze is currently active.
  bool isTimerFrozen() const;


  long remainingMs() const;
  FormattedTime format() const;
  TimerState state() const;
  TimerSession session() const;

private:
  // Calculate the remaining time at the given moment.
  long computeRemainingMs(unsigned long now) const;
  unsigned long startMs_ = 0;
  unsigned long focusMs_ = 5000;
  unsigned long breakMs_ = 3000;
  long remainingMs_ = focusMs_;

  TimerState state_ = TimerState::RUNNING;
  TimerSession session_ = TimerSession::FOCUS;

  // True right after a session finishes.
  // this will keep the timer at `0:00` to avoid instantly jumping to the next session.
  bool modeJustEnded_ = false;
  unsigned long modeEndedAt_ = 0;

  unsigned long pausedAt_ = 0;

  unsigned long timerFronzen_ = false;
  unsigned long timerFronzenAt_ = 0;
  // True when ending the freeze should restore elapsed frozen time.
  unsigned long timerFronzeCompensatesTime_ = false;
};
