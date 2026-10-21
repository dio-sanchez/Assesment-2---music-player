#include <stdint.h> //inclueaded for specific integrer types and lengths
#include <stdio.h>  //included for input and outut functions
#include <avr/interrupt.h>
#include "timer.h"
#include "display.h"
#include "globals.h"


volatile uint8_t pb_debounced_state = 0xFF;
volatile uint16_t note_duration_5ms = 0;
volatile uint8_t fetch_next_note = 0;


extern volatile uint8_t left_byte;
extern volatile uint8_t right_byte;

ISR(TCB1_INT_vect)
{
    static uint8_t count0 = 0;
    static uint8_t count1 = 0;

    uint8_t pb_sample = PORTA.IN;
    uint8_t pb_changed = pb_sample ^ pb_debounced_state;

    count1 = (count1 ^ count0) & pb_changed;
    count0 = ~count0 & pb_changed;

    pb_debounced_state ^= count1 & count0;
    //debouncing logig above


    //counter, just to check how long its been playing  
    if (note_duration_5ms > 0) {
        note_duration_5ms--;
        if (note_duration_5ms == 0) {
            fetch_next_note = 1; // Flag main loop to get the next note
        }
    }


    static uint8_t digit_toggle = 0;
    
    // Pull DISP_LATCH low before sending new data
    PORTA.OUTCLR = PIN1_bm; 


    if (state != PLAYING) {
     swap_display_digit();
    } //keeps running to keep the both numbers running, swapping the display at 100hz(?)

    // Clear interrupt flag.
   TCB1.INTFLAGS = TCB_CAPT_bm;
}
