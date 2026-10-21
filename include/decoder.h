#ifndef DECODER_H
#define DECODER_h
#endif

uint8_t base64_decode_char(char c);
void decode_block(const char *input_progmem_ptr, uint8_t *out_bytes);
uint8_t descramble_byte(uint8_t decoded_byte, uint32_t *lfsr);
uint16_t calculate_frequency(uint8_t octave, uint8_t note);