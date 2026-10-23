#ifndef GLOBALS_H
#define GLOBALS_H

typedef enum
{
  TRACK_SELECT,
  PLAYING,
  PAUSED
} machine_state;

extern uint8_t selected_track;
extern uint8_t current_multiplier;
extern machine_state state;

extern machine_state state;
#endif