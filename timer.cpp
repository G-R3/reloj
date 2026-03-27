#include "timer.h";

void Timer::begin(unsigned long now) {
  startMs_ = now;
  remainingMs_ = focusMs_;
  session_ = TimerSession::FOCUS;
  state_ = TimerState::RUNNING;
  modeEndedAt_ = 0;
  modeJustEnded_ = false;
  pausedAt_ = 0;
}

void Timer::update(unsigned long now) {
  if (modeJustEnded_) {
    // render 0:00 for 1 second before transitioning to the next mode
    if (now - modeEndedAt_ >= 1000) {
      session_ = session_ == TimerSession::FOCUS ? TimerSession::BREAK : TimerSession::FOCUS;
      startMs_ = now;
      remainingMs_ = computeRemainingMs(now);
      modeJustEnded_ = false;
    }
  } else {
    remainingMs_ = computeRemainingMs(now);

    if (remainingMs_ <= 0) {
      remainingMs_ = 0;
      modeEndedAt_ = now;
      modeJustEnded_ = true;
    }
  }
}

void Timer::pause(unsigned long now) {
  if (state_ == TimerState::RUNNING) {
    pausedAt_ = now;
    state_ = TimerState::PAUSED;
  } else {
    unsigned long pausedFor = now - pausedAt_;

    startMs_ += pausedFor;

    if (modeJustEnded_) {
      modeEndedAt_ += pausedFor;
    }

    state_ = TimerState::RUNNING;
  }
}

long Timer::remainingMs() const {
  return remainingMs_;
}

long Timer::computeRemainingMs(unsigned long now) const {
  long time;
  unsigned long elapsedTime = now - startMs_;

  if (session_ == TimerSession::FOCUS) {
    time = focusMs_ - elapsedTime;
  } else {
    time = breakMs_ - elapsedTime;
  }

  return time;
}

FormattedTime Timer::format() const {
  FormattedTime t;

  long totalSeconds = (remainingMs_ + 999) / 1000;
  t.seconds = totalSeconds % 60;
  t.minutes = totalSeconds / 60;

  return t;
}

void Timer::reset(unsigned long now) {
  startMs_ = now;

  if (state_ == TimerState::PAUSED) {
    pausedAt_ = now;
    remainingMs_ = Timer::computeRemainingMs(now);
  }

  // we want to be able to reset during the transitioning phase (rendering 0:00).
  modeEndedAt_ = 0;
  modeJustEnded_ = false;
}

void Timer::setDurations(unsigned long focusMs, unsigned long breakMs) {
  focusMs_ = focusMs;
  breakMs_ = breakMs;
}

TimerSession Timer::session() const {
  return session_;
}

TimerState Timer::state() const {
  return state_;
}
