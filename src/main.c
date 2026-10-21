// system headers
#include <stdint.h>
#include <stdio.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>

// custom headers
#include "data.h"
#include "initialisation.h"
#include "buzzer.h"
#include "display.h"
#include "timer.h"
#include "decoder.h"
#include "globals.h"

// curent state of the machine to start in select track
machine_state state = TRACK_SELECT;

// marks the initial state of the buttons, not pressed so hi
uint8_t pb_state = 0xFF;
uint8_t pb_falling_edge = 0x00;

// imports the varaible for the duation and the next notes from the timer
extern volatile uint16_t note_duration_5ms;
extern volatile uint8_t fetch_next_note;

// curent terack number, starts from 0
uint16_t current_track_index = 1;
uint8_t selected_track = 1; // 1 to 4
uint8_t current_multiplier = 1;

// my student number as the lfsr key
uint32_t lfsr_state = 0x12412821;

// marks the segment table for the display as an extern
extern uint8_t segment_map[];
uint8_t left_digit;
uint8_t right_digit;

uint8_t digit2, digit1, digit0;

void state_machine(void)
{
  while (1)
  {
    // read and save the state of the buttons
    uint8_t pb_state_old = pb_state;
    pb_state = pb_debounced_state;
    pb_falling_edge = pb_state_old & ~pb_state;
    uint8_t pb_rising_edge = ~pb_state_old & pb_state;

    switch (state)
    {
      ///////////////////////// case track select //////////////////////////////////
    case TRACK_SELECT:
      ////////button 1 pressed/////////
      if (!(pb_state & PIN4_bm)) // while the button is held down show the current calue of the pot
      {
        uint8_t preview_multi = (ADC0.RESULT >> 4) + 1;
        find_hex_digits(preview_multi, &digit1, &digit0);
      }
      else
      {
        if (pb_rising_edge & PIN4_bm) // when the button is released, apply the current position of the pot
        {
          current_multiplier = (ADC0.RESULT >> 4) + 1;
        }

        find_hex_digits(selected_track, &digit1, &digit0);

        /////////button 2 /////////////
        if (pb_falling_edge & PIN5_bm)
        {
          if (selected_track > 1)
          {
            selected_track -= 1;
            printf("track selected %d/n", selected_track);
          }
        }
        ///////////button 3//////////
        if (pb_falling_edge & PIN6_bm)
        {
          if (selected_track < 4)
          {
            selected_track += 1;
            printf("track selected %d/n", selected_track);
          }
        }
      }
      //////////button 4////////////
      if (pb_falling_edge & PIN7_bm)
      {
        state = PLAYING;
        current_track_index = track_start[selected_track - 1] * 4; // example, to be changed later when implementing the track selection logic
        if (selected_track == 4)
        {
          lfsr_state = 0x12412821;
        }
        fetch_next_note = 1;
        printf("PLAYING track %d\n", current_track_index);
      }
      break;

      //////////////////////// case playing /////////////////////////////////////

    case PLAYING:
      if (pb_falling_edge & PIN6_bm)
      {
        state = PAUSED;
        buzzer_stop(); // Stop the buzzer when pause
        printf("PAUSED\n");
      }
      else if (fetch_next_note) // if it isnt paused just keep playing
      {
        fetch_next_note = 0;

        // 1. Fetch 4 Encoded Characters
        char b64_chars[4];
        for (int i = 0; i < 4; i++)
        {
          b64_chars[i] = pgm_read_byte(&encoded_data[current_track_index + i]);
        }

        // 2. Decode into 3 bytes
        uint8_t decoded_bytes[3];
        decode_block(b64_chars, decoded_bytes);

        // 3. Descramble if Track 4 is playing
        if (selected_track == 4)
        {
          decoded_bytes[0] = descramble_byte(decoded_bytes[0], &lfsr_state);
          decoded_bytes[1] = descramble_byte(decoded_bytes[1], &lfsr_state);
          decoded_bytes[2] = descramble_byte(decoded_bytes[2], &lfsr_state);
        }

        // 4. Check for Terminating Instruction (0x00 0x00 0x00)
        if (decoded_bytes[0] == 0 && decoded_bytes[1] == 0 && decoded_bytes[2] == 0)
        {
          state = TRACK_SELECT;
          buzzer_stop();

          // Advance to next track, unless currently on Track 4
          if (selected_track < 4)
            selected_track++;
          continue;
        }

        // 5. Parse the Instruction
        uint8_t duration = decoded_bytes[0];
        uint8_t octave = decoded_bytes[1] >> 4;
        uint8_t note = decoded_bytes[1] & 0x0F;
        uint8_t display = decoded_bytes[2];

        // 6. Apply Pitch, Duration, and Display
        uint16_t freq = calculate_frequency(octave, note);
        buzzer_play_freq(freq);
        note_duration_5ms = duration * current_multiplier;

        // Write display byte directly to shift register without modification
        PORTA.OUTCLR = PIN1_bm; // Latch low
        SPI0.DATA = display;    // Send byte
        // Your SPI ISR will set Latch high once transmission completes

        current_track_index += 4; // Advance to next 4-character block
      }
      break;

      if (pb_falling_edge & PIN7_bm)
      {
        state = TRACK_SELECT;
        printf("TRACK SELECT\n");
      }
      break;

      //////////////////////////// case paused ///////////////////////////////

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
  spi_init(); // serial peripheral interface
  sei();

  state_machine();

  // The program should not reach this point
  while (1)
    ;
}