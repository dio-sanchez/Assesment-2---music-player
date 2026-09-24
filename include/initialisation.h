#ifndef INITIALISATION_H
#define INITIALISATION_H

//defining states
enum states{
    TRACK_SELECT,
    PLAYING,
    PAUSED
} State;

typedef enum events{
    S1_HOLD,
    S1_RELEASED,
    S2_PRESSED,
    S3_PRESSED,
    S4_PRESSED
} Events;
#endif



