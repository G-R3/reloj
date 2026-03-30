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
};
