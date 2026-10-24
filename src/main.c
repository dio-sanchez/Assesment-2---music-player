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
#include "uart.h"
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
uint16_t current_full_duration = 0;

// my student number as the lfsr key
uint32_t lfsr_state = 0x12412821;

// marks the segment table for the display as an extern
extern uint8_t segment_map[];
uint8_t left_digit = 0x00;
uint8_t right_digit = 0x00;

uint8_t digit2, digit1, digit0;

// parses states of uart
typedef enum
{
  PARSER_IDLE,
  PARSER_IDENTIFIER,
  PARSER_PAYLOAD
} parser_state_t;

parser_state_t serial_state = PARSER_IDLE;
char command_id = 0;
char payload[32];
uint8_t payload_index = 0;

volatile uint8_t serial_edges = 0;

// stops theb buzzer and playback
void stop_playback(void)
{
  buzzer_stop();
  cli();
  note_duration_5ms = 0;
  fetch_next_note = 0;
  sei();
  current_full_duration = 0;
  state = TRACK_SELECT;
}
void start_note_timer(void)
{
  cli();
  note_duration_5ms = current_full_duration;
  sei();
  if (current_full_duration == 0) // zero-length instruction: move on immediately
    fetch_next_note = 1;
}

// Track 4 seek: jump to the instruction, advance the LFSR to match, apply it.
void seek_track4(uint8_t seek_value)
{
  uint16_t offset = 8 * (uint16_t)seek_value;

  current_track_index = (track_start[3] + offset) * 4;
  lfsr_state = 0x12412821;
  for (uint16_t i = 0; i < offset * 3; i++)
    descramble_byte(0, &lfsr_state);

  apply_instruction(); // stays paused; timer isn't started
}

// Common end-of-track handling (terminator reached).
void finish_track(void)
{
  // helper from fix 2
  if (selected_track < 4)
  {
    selected_track++;
    current_track_index = track_start[selected_track - 1] * 4;
    fetch_next_note = 1;
  }
  else
    stop_playback();
}
uint8_t apply_instruction(void)
{
  uint8_t ins[3];
  fetch_instruction(current_track_index, selected_track == 4, &lfsr_state, ins);
  current_track_index += 4;
  return execute_instruction(ins, current_multiplier, &current_full_duration);
}

void process_serial_command(char id, char *payload_data)
{
  switch (id)
  {
  case 't':
    // TODO: Decode and descramble test_message
    // printf("? %s\n", decoded_text);
    printf("#ACK\n");
    break;

  case '2':
    if (state == PLAYING)
    {
      printf("#NACK\n"); // S2 is inactive during playback[cite: 18]
    }
    else
    {
      serial_edges |= PIN5_bm; // Simulate S2 falling edge
      printf("#ACK\n");
    }
    break;

  case '3':
    serial_edges |= PIN6_bm; // Simulate S3 falling edge
    printf("#ACK\n");
    break;

  case '4':
    serial_edges |= PIN7_bm; // Simulate S4 falling edge
    printf("#ACK\n");
    break;

  case 'm':
    if (state != TRACK_SELECT)
    {
      printf("#NACK\n"); // Only valid in TRACK_SELECT[cite: 18]
    }
    else
    {
      // TODO: Add Base64 validation and instruction playback logic here[cite: 17, 18]
      printf("#ACK\n");
    }
    break;

  default:
    printf("#NACK\n"); // Unknown identifier[cite: 18]
    break;
  }
}

void state_machine(void)
{
  while (1)
  {
    // read and save the state of the buttons
    uint8_t pb_state_old = pb_state;
    pb_state = pb_debounced_state;
    pb_falling_edge = pb_state_old & ~pb_state;
    serial_edges = 0;
    uint8_t pb_rising_edge = ~pb_state_old & pb_state;
    uint8_t s1_held = !(pb_state & PIN4_bm);

    switch (state)
    {
      ///////////////////////// case track select //////////////////////////////////
    case TRACK_SELECT:
      ////////button 1 pressed/////////
      if (s1_held) // while the button is held down show the current calue of the pot
      {
        find_hex_digits((ADC0.RESULT >> 4) + 1, &digit1, &digit0);
      }
      else
      {
        if (pb_rising_edge & PIN4_bm) // when the button is released, apply the current position of the pot
        {
          current_multiplier = (ADC0.RESULT >> 4) + 1;
        }

        find_hex_digits(selected_track, &digit1, &digit0);

        /////////button 2 /////////////
        if ((pb_falling_edge & PIN5_bm) && selected_track > 1)
          selected_track--;

        ///////////button 3//////////
        if ((pb_falling_edge & PIN6_bm) && selected_track < 4)
          selected_track++;

        //////////button 4////////////
        if (pb_falling_edge & PIN7_bm)
        {
          current_track_index = track_start[selected_track - 1] * 4; // example, to be changed later when implementing the track selection logic
          if (selected_track == 4)
            lfsr_state = 0x12412821;
          state = PLAYING;
          fetch_next_note = 1;
        }
      }
      break;

      //////////////////////// case playing /////////////////////////////////////

    case PLAYING:
      if (pb_falling_edge & PIN6_bm)
      {
        stop_playback();
      }
      else if (fetch_next_note)
      {
        fetch_next_note = 0;
        if (apply_instruction())
        {
          finish_track();
          break;
        }
        start_note_timer();
        // pending s4 pressed at the same moment, handled in the next loo :)
      }
      else if (pb_falling_edge & PIN7_bm)
      {
        state = PAUSED;
      }
      break;
      //////////////////////////// case paused ///////////////////////////////

    case PAUSED:
    {
      uint8_t seekable = (selected_track == 4);

      if (seekable && s1_held)
      {
        find_hex_digits(ADC0.RESULT, &digit1, &digit0);
        break; // other buttons ignored while S1 held
      }
      if (seekable && (pb_rising_edge & PIN4_bm))
      {
        seek_track4(ADC0.RESULT);
        break;
      }

      if (pb_falling_edge & PIN6_bm)
      {
        stop_playback();
      }
      else if (pb_falling_edge & PIN5_bm)
      {
        if (apply_instruction())
          finish_track();
      }
      else if (pb_falling_edge & PIN7_bm)
      {
        state = PLAYING;
        start_note_timer(); // full duration again
      }
      break;
    }
    }

    char c;
    if (uart_take_char(&c))
    {
      switch (serial_state)
      {
      case PARSER_IDLE:
        if (c == '\\')
        {
          serial_state = PARSER_IDENTIFIER;
        }
        break;

      case PARSER_IDENTIFIER:
        command_id = c;
        if (command_id == 'm')
        {
          payload_index = 0;
          serial_state = PARSER_PAYLOAD;
        }
        else
        {
          process_serial_command(command_id, NULL);
          serial_state = PARSER_IDLE;
        }
        break;

      case PARSER_PAYLOAD:
        if (command_id == 'm')
        {
          payload[payload_index++] = c;
          if (payload_index == 32)
          {
            process_serial_command(command_id, payload);
            serial_state = PARSER_IDLE;
          }
        }
        break;
      }
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
  uart_init();
  stdio_init();
  sei();

  state_machine();

  // The program should not reach this point
  while (1)
    ;
}