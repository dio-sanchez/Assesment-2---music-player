#ifndef GLOBALS_H
#define GLOBALS_H

typedef enum
{
  TRACK_SELECT,
  PLAYING,
  PAUSED
} machine_state;

extern machine_state state;
#endif