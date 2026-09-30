#include <stdint.h>     //inclueaded for specific integrer types and lengths
#include <stdio.h>      //included for input and outut functions
#include "data.c"
#include <avr/interrupt.h>
#include "initialisation.h"

typedef enum {
    TRACK_SELECT,
    PLAYING,
    PAUSED
} machine_state;

machine_state state = TRACK_SELECT;


void state_machine(void) {
  while (1) {
   switch (state)
   {
   case TRACK_SELECT:
    if (!(PORTA.IN & PIN4_bm))
    {
      state = PLAYING;
      display('Play');
    }
    break;


   case PLAYING:
    if (!(PORTA.IN & PIN6_bm))
    {
      state = PAUSED;
      display('Pause');
    } else if (!(PORTA.IN & PIN7_bm)) {
      state = TRACK_SELECT;
      display('Selecting');
    }
    break;
    

    case PAUSED:
    if (!(PORTA.IN & PIN4_bm))
    {
      state = PLAYING;
      display('Play');
    } else if (!(PORTA.IN & PIN7_bm)) {
      state = TRACK_SELECT;
      display('Selecting');
    }
    break;

   default:
    break;
   }
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