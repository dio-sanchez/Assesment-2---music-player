#include <stdint.h> //inclueaded for specific integrer types and lengths
#include <stdio.h>  //included for input and outut functions
#include "data.c"
#include <avr/interrupt.h>
#include "initialisation.h"

typedef enum
{
  TRACK_SELECT,
  PLAYING,
  PAUSED
} machine_state;

machine_state state = TRACK_SELECT;

void state_machine(void)
{
  while (1)
  {
    switch (state)
    {
    case TRACK_SELECT:
      if (!(PORTA.IN & PIN7_bm))
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
      }
      else if (!(PORTA.IN & PIN7_bm))
      {
        state = TRACK_SELECT;
        display('Selecting');
      }
      break;

    case PAUSED:
      if (!(PORTA.IN & PIN6_bm))
      {
        state = PLAYING;
        display('Play');
      }
      else if (!(PORTA.IN & PIN7_bm))
      {
        state = TRACK_SELECT;
        display('Selecting');
      }
      break;

    default:
      break;
    }
  }
}

int main(void)
{
  cli();
  buttons_init();
  display_init();
  timer_init();
  adc_init(); // potentiometer
  pwm_init(); //pulse width modulation
  sei();

  state_machine();

  while (state = PLAYING)
  {
    /* code to play music, run the decoder with the current song (seek value) and multiplier 
    for this the only periferals being used are the buzzer and the timer*/
  }

  while (state = PAUSED)
  {
    while (PORTA.IN & PIN4_bm)
    {
      /* activate potentiometer */
    }

    if (!(PORTA.IN & PIN5_bm))
    {
      /* skip track, apply next instruction */
    }
    
  }
  
  while (state = TRACK_SELECT)
  {
    if (!(PORTA.IN & PIN5_bm))
    {
      /* prev track */
    }
    if (!(PORTA.IN & PIN6_bm))
    {
      /* next track */
    }
    while (PORTA.IN & PIN4_bm)
    {
      /* activate potentiometer */
    }
    
    
  }
  
  
  // The program should not reach this point
  while (1)
    ;
}