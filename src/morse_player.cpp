#include "morse_player.hpp"

#if defined(__CLING__)
#include <stdio.h>
#define DBG(msg) printf(msg)
#else
#define DBG(msg) 
#endif


Player::Player(unsigned long duration_unit, state_changed_fun_ptr state_changed_cb, millis_fun_ptr millis_cb) {
    this->duration_unit = duration_unit;
    this->state_changed_cb = state_changed_cb;
    this->millis_cb = millis_cb;
}

void Player::playDot() {
  play(SIGNAL_DOT);
}

void Player::playDash() {
  play(SIGNAL_DASH);
}

void Player::play(SignalType signal_type) {
  switch (signal_type) {
    case SIGNAL_DOT:
      if (!(play_state == PLAY_STATE_DOT_ON || play_state == PLAY_STATE_DOT_OFF))
          setPlayState(PLAY_STATE_DOT_ON);
      break;
    case SIGNAL_DASH:
      if (!(play_state == PLAY_STATE_DASH_ON || play_state == PLAY_STATE_DASH_OFF))
        setPlayState(PLAY_STATE_DASH_ON);
      break;
  }
}

void Player::stop() {
  setPlayState(PLAY_STATE_STOPPED);
}

void Player::setPlayState(PlayState play_state) {
  this->play_state = play_state;
  last_state_change = millis_cb();
  state_changed_cb(play_state);
}

PlayState Player::getPlayState() {
  return play_state;
}

unsigned long Player::getPlayStateAge() {
  return millis_cb() - last_state_change;
}

unsigned long Player::getPlayStateDuration() {
  switch (play_state) {
    case PLAY_STATE_DOT_ON:
        return duration_unit;
    case PLAY_STATE_DASH_ON:
        return duration_unit * 3;
    case PLAY_STATE_DOT_OFF:
    case PLAY_STATE_DASH_OFF:
        return duration_unit;
    case PLAY_STATE_STOPPED:
    case PLAY_STATE_UNSET:
        return 0;
  }
}

bool Player::isPlayStateExpired() {
  bool expired = false;
  switch (play_state) {
    case PLAY_STATE_UNSET:
        expired = true;
    default:
        expired = getPlayStateAge() > getPlayStateDuration();
  }

  DBG("Play state ");
  DBG(play_state_str[play_state]);
  DBG(" expired? ");
  DBG(expired ? "yes" : "no");
  DBG("\n");

  return expired;
}

PlayState Player::getNextPlayState() {
  switch (play_state) {
    case PLAY_STATE_STOPPED:
        return PLAY_STATE_STOPPED;
    case PLAY_STATE_DOT_ON:
        return PLAY_STATE_DOT_OFF;
    case PLAY_STATE_DOT_OFF:
        return  PLAY_STATE_DOT_ON;
    case PLAY_STATE_DASH_ON:
        return  PLAY_STATE_DASH_OFF;
    case PLAY_STATE_DASH_OFF:
        return  PLAY_STATE_DASH_ON;
    default:
        return PLAY_STATE_UNSET;
  }
}

PlayState Player::switchPlayState() {
  PlayState next_play_state = getNextPlayState();
  setPlayState(next_play_state);
  return next_play_state;
}

bool Player::step() {
  PlayState next_play_state;
  bool ready_to_switch = false;
  if (isPlayStateExpired()) {
    next_play_state = switchPlayState();
    ready_to_switch = !(next_play_state == PLAY_STATE_DOT_OFF || next_play_state == PLAY_STATE_DASH_OFF);
  }
  return ready_to_switch;
}
