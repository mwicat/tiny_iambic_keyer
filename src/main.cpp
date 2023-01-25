#include <stdio.h>
#include <Arduino.h>

#include "morse_player.hpp"

#include "dds.hpp"


const int relay_out_pin = 2;

const int dot_in_pin = 3;
const int dash_in_pin = 4;

bool play_tone = false;
bool dot_btn_pushed = false;
bool dash_btn_pushed = false;
bool ready_to_switch = false;

Player* player;


void switchRelay(bool on) {
  digitalWrite(relay_out_pin, on ? LOW : HIGH);
}

void play_state_changed_cb(PlayState play_state) {
  switch (play_state) {
    case PLAY_STATE_DOT_ON:
    case PLAY_STATE_DASH_ON:
      play_tone = true;
      break;
    case PLAY_STATE_DOT_OFF:
    case PLAY_STATE_DASH_OFF:
    case PLAY_STATE_STOPPED:
    case PLAY_STATE_UNSET:
      play_tone = false;
      break;
  }
  switchRelay(play_tone);
}


void setup() {
  pinMode(relay_out_pin, OUTPUT);
  pinMode(dot_in_pin, INPUT_PULLUP);
  pinMode(dash_in_pin, INPUT_PULLUP);
  switchRelay(false);
  Freq = 700;
  CalculateSine();
  SetupDDS();
  Jump = Freq*4;
  player = new Player(100 * 30, &play_state_changed_cb, &millis);
}

ISR(TIMER0_COMPA_vect) {
  if (play_tone) {
    Sine();
  }
}

void loop() {
  if (ready_to_switch) {
    dot_btn_pushed = (digitalRead(dot_in_pin) == LOW);
    dash_btn_pushed = (digitalRead(dash_in_pin) == LOW);

    if (dot_btn_pushed) {
      player->playDot();
    } else if (dash_btn_pushed) {
      player->playDash();
    } else {
      player->stop();
    }
  }

  ready_to_switch = player->step();
}
