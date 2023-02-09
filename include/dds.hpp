#ifndef DDS_HPP
#define DDS_HPP

#include <Arduino.h>

#define NOINIT __attribute__ ((section (".noinit")))

unsigned int Freq NOINIT;
int8_t Sinewave[256] NOINIT;

volatile unsigned int Acc, Jump;


void CalculateSine () {
  int X=0, Y=8180;
  for (int i=0; i<256; i++) {
    X = X + (Y*4)/163;
    Y = Y - (X*4)/163;
    Sinewave[i] = X>>6;
  }
}

void Sine () {
  Acc = Acc + Jump;
  OCR1A = Sinewave[Acc>>8] + 128;
}

void SetupDDS () {
  // Enable 64 MHz PLL and use as source for Timer1
  PLLCSR = 1<<PCKE | 1<<PLLE;

  // Set up Timer/Counter1 for PWM output
  TIMSK = 0;                               // Timer interrupts OFF
  TCCR1 = 1<<PWM1A | 2<<COM1A0 | 1<<CS10;  // PWM A, clear on match, 1:1 prescale
  pinMode(1, OUTPUT);                      // Enable PWM output pin

  // Set up Timer/Counter0 for 20kHz interrupt to output samples.
  TCCR0A = 3<<WGM00;                       // Fast PWM
  TCCR0B = 1<<WGM02 | 2<<CS00;             // 1/8 prescale (1MHz)
  //TIMSK = 1<<OCIE0A;                     // Enable compare match, disable overflow
  TIMSK = 1<<OCIE0A | 1<<TOIE0;            // Enable compare match, disable overflow

  //   The Output Compare Register A contains an 8-bit value that is continuously compared with the counter value
  // (TCNT0). A match can be used to generate an Output Compare interrupt, or to generate a waveform output on the
  // OC0A pin.
  OCR0A = 60;                              // Divide by 61 - Timer/Counter0 – Output Compare Register A
}
#endif