#ifndef MORSE_PLAYER_HPP
#define MORSE_PLAYER_HPP

enum SignalType {
  SIGNAL_DOT,
  SIGNAL_DASH,
};

enum PlayState {
  PLAY_STATE_UNSET,
  PLAY_STATE_STOPPED,
  PLAY_STATE_DOT_ON,
  PLAY_STATE_DOT_OFF,
  PLAY_STATE_DASH_ON,
  PLAY_STATE_DASH_OFF,
};

const static char* play_state_str[] = {
    "unset",
    "stopped",
    "dot-on",
    "dot-off",
    "dash-on",
    "dash-off"
};

typedef unsigned long (*millis_fun_ptr)();
typedef void (*state_changed_fun_ptr)(PlayState);

class Player {
  public:
    Player(unsigned long duration_unit, state_changed_fun_ptr state_changed_cb, millis_fun_ptr millis_cb);
    bool step();
    void playDash();
    void playDot();
    void play(SignalType signal_type);
    void stop();
    PlayState getPlayState();
    unsigned long getPlayStateAge();
    PlayState getNextPlayState();
    unsigned long getPlayStateDuration();
    void setPlayState(PlayState state);
    bool isPlayStateExpired();
    PlayState switchPlayState();
  private:
    millis_fun_ptr millis_cb;
    state_changed_fun_ptr state_changed_cb;
    PlayState play_state = PLAY_STATE_UNSET;
    unsigned long last_state_change = 0;
    unsigned long int duration_unit;
};
#endif