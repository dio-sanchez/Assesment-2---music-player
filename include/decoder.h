#ifndef DECODER_H
#define DECODER_h

uint8_t apply_instruction(void);
void seek_track4(uint8_t seek_value);
void start_note_timer(void);
void finish_track(void);
uint8_t descramble_byte(uint8_t decoded_byte, uint32_t *lfsr);
void stop_playback(void);
void fetch_instruction(uint16_t index, uint8_t track4, uint32_t *lfsr, uint8_t ins[3]);
uint8_t execute_instruction(const uint8_t ins[3], uint8_t multiplier, uint16_t *full_duration);

#endif