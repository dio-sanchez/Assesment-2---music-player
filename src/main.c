// system headers
#include <stdint.h>
#include <stdio.h>
#include <avr/io.h>
#include <avr/interrupt.h>

// custom headers
#include "data.h"
#include "initialisation.h"
#include "buzzer.h"
#include "display.h"
#include "timer.h"

typedef enum
{
  TRACK_SELECT,
  PLAYING,
  PAUSED
} machine_state;


machine_state state = TRACK_SELECT;


uint8_t pb_state = 0xFF;
uint8_t pb_falling_edge = 0x00;

void state_machine(void)
{
  while (1)
  {
    // read and save the state of the buttons
    uint8_t pb_state_old = pb_state;

    pb_state = pb_debounced_state;

    pb_falling_edge = pb_state_old & ~pb_state;



    switch (state)
    {
    case TRACK_SELECT:
      if (pb_falling_edge & PIN7_bm)
      {
        state = PLAYING;
        printf("PLAYING\n");
      }
      if (!(PORTA.IN & PIN4_bm)) // while the button is pressed activate the potentiometer ****** need to figure out how to make it work with the debounced
      {
        /* code */
      }
      if (pb_falling_edge & PIN5_bm)
      {
        /* code */
      }
      if (pb_falling_edge & PIN6_bm)
      {
        /* code */
      }

      break;

    case PLAYING:
      if (pb_falling_edge & PIN6_bm)
      {
        state = PAUSED;
        printf("PAUSED\n");
      }
      else if (pb_falling_edge & PIN7_bm)
      {
        state = TRACK_SELECT;
        printf("TRACK SELECT\n");
      }
      break;

    case PAUSED:
      if (pb_falling_edge & PIN6_bm)
      {
        state = PLAYING;
        printf("PLAYING\n");
      }
      else if (pb_falling_edge & PIN7_bm)
      {
        state = TRACK_SELECT;
        printf("TRACK SELECT\n");
      }
      if (PORTA.IN & PIN4_bm) // while the button is pressed activate the potentiometer
      {
        /* code */
      }
      if (pb_falling_edge & PIN5_bm)
      {
        /* code */
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
  pwm_init(); // pulse width modulation
  sei();

  


  state_machine();

  // The program should not reach this point
  while (1)
    ;
}