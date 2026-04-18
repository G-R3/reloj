#include "timer.h"

namespace {
constexpr unsigned long transitionDelayMs = 1000;
}

void Timer::begin(unsigned long now) {
  startMs_ = now;
  remainingMs_ = static_cast<long>(focusMs_);
  session_ = TimerSession::FOCUS;
  state_ = TimerState::RUNNING;
  modeEndedAt_ = 0;
  modeJustEnded_ = false;
  pausedAt_ = 0;

  timerFrozen_ = false;
  freezeCompensatesElapsed_ = false;
  timerFrozenAt_ = 0;
}

void Timer::update(unsigned long now) {

  if (state_ == TimerState::PAUSED || timerFrozen_) return;

  if (modeJustEnded_) {
    // render 0:00 for 1 second before transitioning to the next mode
    if (now - modeEndedAt_ >= transitionDelayMs) {
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

void Timer::togglePause(unsigned long now) {
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
  const unsigned long elapsedTime = now - startMs_;
  return static_cast<long>(sessionDurationMs()) - static_cast<long>(elapsedTime);
}

unsigned long Timer::sessionDurationMs() const {
  return session_ == TimerSession::FOCUS ? focusMs_ : breakMs_;
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
  }

  remainingMs_ = Timer::computeRemainingMs(now);
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

void Timer::beginTimerFreeze(unsigned long now) {

  if (timerFrozen_) return;

  timerFrozen_ = true;
  timerFrozenAt_ = now;
  freezeCompensatesElapsed_ = state_ == TimerState::RUNNING;
}


// Reimburse elapsed time when a temporary hold action should not consume timer time. Should come in handy if I ever resume the timer screen after a hold.
void Timer::endTimerFreeze(unsigned long now, bool compensateElapsed) {
  if (!timerFrozen_) return;

  if (compensateElapsed && freezeCompensatesElapsed_) {
    const unsigned long frozenFor = now - timerFrozenAt_;

    startMs_ += frozenFor;

    if (modeJustEnded_) {
      modeEndedAt_ += frozenFor;
    }
  }

  timerFrozen_ = false;
  timerFrozenAt_ = 0;
  freezeCompensatesElapsed_ = false;
}

void Timer::skip(unsigned long now) {
  session_ = session_ == TimerSession::FOCUS ? TimerSession::BREAK : TimerSession::FOCUS;

  startMs_ = now;
  remainingMs_ = computeRemainingMs(now);
  modeJustEnded_ = false;
  modeEndedAt_ = 0;

  if (state_ == TimerState::PAUSED) {
    pausedAt_ = now;
  }
}

bool Timer::isTimerFrozen() const {
  return timerFrozen_;
}

bool Timer::hasSessionEnded() const {
  return modeJustEnded_;
}