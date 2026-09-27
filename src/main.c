#include <stdint.h>     //inclueaded for specific integrer types and lengths
#include <stdio.h>      //included for input and outut functions
#include "data.c"
#include <avr/interrupt.h>
#include "initialisation.h"

extern enum {
    TRACK_SELECT,
    PLAYING,
    PAUSED
} State;

void state_machine(void) {
  while (1) {
    // Implement your main loop here
  }
}

int main(void) {
  cli();
  // Call your initialisation functions here
  sei();

  state_machine();

  // The program should not reach this point
  while (1)
    ;
}