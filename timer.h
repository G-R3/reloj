enum class State { Paused,
                   Running };
enum class Session { Focus,
                     Break };

struct FormattedTime {
  int minutes;
  int seconds;
};

class Timer {
public:
  void begin(unsigned long now);
  void update(unsigned long now);
  void pause(unsigned long now);
  void reset(unsigned long now);
  void setDurations(unsigned long focusMs, unsigned long breakMs);

  long remainingMs() const;
  FormattedTime format() const;
  State state() const;
  Session session() const;

private:
  long computeRemainingMs(unsigned long now) const;
  unsigned long startMs_;
  unsigned long focusMs_ = 5000;
  unsigned long breakMs_ = 3000;
  long remainingMs_ = focusMs_;
  
  State state_ = State::Running;
  Session session_ = Session::Focus;

  bool modeJustEnded_ = false;
  unsigned long modeEndedAt_ = 0;

  unsigned long pausedAt_;
};
