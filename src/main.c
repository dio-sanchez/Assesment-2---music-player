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

// stops theb buzzer and playback
static void stop_playback(void)
{
  buzzer_stop();
  cli();
  note_duration_5ms = 0;
  fetch_next_note = 0;
  sei();
  current_full_duration = 0;
  state = TRACK_SELECT;
}

static uint8_t apply_instruction(void)
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
      pb_falling_edge |= PIN5_bm; // Simulate S2 falling edge
      printf("#ACK\n");
    }
    break;

  case '3':
    pb_falling_edge |= PIN6_bm; // Simulate S3 falling edge
    printf("#ACK\n");
    break;

  case '4':
    pb_falling_edge |= PIN7_bm; // Simulate S4 falling edge
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
          }
        }
        ///////////button 3//////////
        if (pb_falling_edge & PIN6_bm)
        {
          if (selected_track < 4)
          {
            selected_track += 1;
          }
        }
        if (pb_falling_edge & PIN7_bm)
        {
          state = PLAYING;
          current_track_index = track_start[selected_track - 1] * 4; // example, to be changed later when implementing the track selection logic
          if (selected_track == 4)
          {
            lfsr_state = 0x12412821;
          }
          if (note_duration_5ms == 0)
            fetch_next_note = 1;
        }
      }
      //////////button 4////////////
      break;

      //////////////////////// case playing /////////////////////////////////////

    case PLAYING:
      if (pb_falling_edge & PIN6_bm)
      {
        stop_playback();
      }
      else if (pb_falling_edge & PIN7_bm)
      {
        state = PAUSED;
      }
      else if (fetch_next_note) // if it isnt paused just keep playing
      {
        fetch_next_note = 0;

        // fetch 4 Encoded Characters
        char b64_chars[4];
        memcpy_P(b64_chars, &encoded_data[current_track_index], 4);

        // decode into 3 bytes
        uint8_t decoded_bytes[3];
        decode_block(b64_chars, decoded_bytes);

        // descramble if Track 4 is playing
        if (selected_track == 4)
        {
          decoded_bytes[0] = descramble_byte(decoded_bytes[0], &lfsr_state);
          decoded_bytes[1] = descramble_byte(decoded_bytes[1], &lfsr_state);
          decoded_bytes[2] = descramble_byte(decoded_bytes[2], &lfsr_state);
        }

        // check for terminating instruction (0x00 0x00 0x00)
        if (decoded_bytes[0] == 0 && decoded_bytes[1] == 0 && decoded_bytes[2] == 0)
        {
          state = TRACK_SELECT;
          buzzer_stop();

          // advance to next track, unless currently on track 4
          if (selected_track < 4)
          {
            selected_track++;
          }
          continue;
        }

        // parse
        uint8_t duration = decoded_bytes[0];
        uint8_t octave = decoded_bytes[1] >> 4;
        uint8_t note = decoded_bytes[1] & 0x0F;
        uint8_t display = decoded_bytes[2];

        // apply pitch duration and siplay

        uint16_t freq = calculate_frequency(octave, note);
        buzzer_play_freq(freq);
        note_duration_5ms = duration * current_multiplier;
        current_full_duration = note_duration_5ms;

        // write display byte directly to shift register without modification
        PORTA.OUTCLR = PIN1_bm; // Latch low
        SPI0.DATA = display;    // Send byte
        // Your SPI ISR will set Latch high once transmission completes

        current_track_index += 4; // Advance to next 4-character block
      }
      break;

      break;
      //////////////////////////// case paused ///////////////////////////////

    case PAUSED:
    
      if ((selected_track == 4) && !(pb_state & PIN4_bm)) // while the button is held down show the current calue of the pot
      {
        pb_falling_edge &= ~(PIN5_bm | PIN6_bm | PIN7_bm);
        uint8_t preview_seek = ADC0.RESULT;
        find_hex_digits(preview_seek, &digit1, &digit0);
      }
      else if ((selected_track == 4) && (pb_rising_edge & PIN4_bm)) // when the button is released, apply the current position of the pot
      {
        uint16_t seek_value = ADC0.RESULT;
        uint16_t instruction_offset = 8 * seek_value;

        current_track_index = (track_start[3] + instruction_offset) * 4;
        lfsr_state = 0x12412821;

        for (uint16_t i = 0; i < (instruction_offset * 3); i++)
        {
          descramble_byte(0, &lfsr_state);
        }
        // same steps as when s2 is pressed
        //  fetch 4 Encoded Characters
        char b64_chars[4];
        memcpy_P(b64_chars, &encoded_data[current_track_index], 4);

        // decode into 3 bytes
        uint8_t decoded_bytes[3];
        decode_block(b64_chars, decoded_bytes);

        // descramble if Track 4 is playing
        if (selected_track == 4)
        {
          decoded_bytes[0] = descramble_byte(decoded_bytes[0], &lfsr_state);
          decoded_bytes[1] = descramble_byte(decoded_bytes[1], &lfsr_state);
          decoded_bytes[2] = descramble_byte(decoded_bytes[2], &lfsr_state);
        }

        // parse
        uint8_t duration = decoded_bytes[0];
        uint8_t octave = decoded_bytes[1] >> 4;
        uint8_t note = decoded_bytes[1] & 0x0F;
        uint8_t display = decoded_bytes[2];

        current_full_duration = duration * current_multiplier;

        // apply pitch duration and siplay
        buzzer_play_freq(calculate_frequency(octave, note));

        PORTA.OUTCLR = PIN1_bm;
        SPI0.DATA = display;

        // advance index for the next operation1111
        current_track_index += 4;
      }

      if (pb_falling_edge & PIN5_bm)
      {
        // fetch 4 Encoded Characters
        char b64_chars[4];
        memcpy_P(b64_chars, &encoded_data[current_track_index], 4);

        // decode into 3 bytes
        uint8_t decoded_bytes[3];
        decode_block(b64_chars, decoded_bytes);

        // descramble if Track 4 is playing
        if (selected_track == 4)
        {
          decoded_bytes[0] = descramble_byte(decoded_bytes[0], &lfsr_state);
          decoded_bytes[1] = descramble_byte(decoded_bytes[1], &lfsr_state);
          decoded_bytes[2] = descramble_byte(decoded_bytes[2], &lfsr_state);
        }

        // check for terminating instruction (0x00 0x00 0x00)
        if (decoded_bytes[0] == 0 && decoded_bytes[1] == 0 && decoded_bytes[2] == 0)
        {
          state = TRACK_SELECT;
          buzzer_stop();

          // advance to next track, unless currently on track 4
          if (selected_track < 4)
            selected_track++;
          continue;
        }
        else
        {

          // parse
          uint8_t duration = decoded_bytes[0];
          uint8_t octave = decoded_bytes[1] >> 4;
          uint8_t note = decoded_bytes[1] & 0x0F;
          uint8_t display = decoded_bytes[2];

          current_full_duration = duration * current_multiplier;

          // apply pitch duration and siplay
          buzzer_play_freq(calculate_frequency(octave, note));

          PORTA.OUTCLR = PIN1_bm;
          SPI0.DATA = display;

          // advance index for the next operation1111
          current_track_index += 4;
        }
      }
      if (pb_falling_edge & PIN6_bm)
      {
        stop_playback();
      }
      else if (pb_falling_edge & PIN7_bm)
      {
        state = PLAYING;
        note_duration_5ms = current_full_duration;

        if (note_duration_5ms == 0)
          fetch_next_note = 1;
      }

      break;

    default:
      break;
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