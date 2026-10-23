#ifndef GLOBALS_H
#define GLOBALS_H

typedef enum
{
  TRACK_SELECT,
  PLAYING,
  PAUSED
} machine_state;


extern machine_state state;

extern uint16_t current_track_index;
extern uint8_t  selected_track;
extern uint8_t  current_multiplier;
extern uint16_t current_full_duration;
extern uint32_t lfsr_state;
extern volatile uint16_t note_duration_5ms;
extern volatile uint8_t  fetch_next_note;

#endif