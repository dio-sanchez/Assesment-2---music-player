#ifndef DATA_H
#define DATA_H

// EGB202 Assessment 2 data file for Diego Sanchez Vial (n12412821).
//
// Do not modify this file. See the Assessment 2 specification on
// Canvas for how to decode and descramble this data.

#include <avr/pgmspace.h>
#include <stdint.h>

extern const uint16_t track_start[4];
extern const char test_message[] PROGMEM;
extern const char encoded_data[] PROGMEM;

#endif