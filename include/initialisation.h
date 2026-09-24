#ifndef INITIALISATION_H
#define INITIALISATION_H

#include <stdint.h>

typedef enum {
    TRACK_SELECT,
    PLAYING,
    PAUSED
} State_t;

typedef enum {
    S1_HOLD,
    S1_RELEASED,
    S2_PRESSED,
    S3_PRESSED,
    S4_PRESSED
} Events;

extern State_t State;

#endif



