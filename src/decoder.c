#include <stdint.h>
#include <avr/pgmspace.h>
#include "data.h"

//function to map ase64 characters and corresponding decoded values.
uint8_t b64_char_to_val(char c) {
    if (c >= 'A' && c <= 'Z') return c - 'A';
    if (c >= 'a' && c <= 'z') return c - 'a' + 26;
    if (c >= '0' && c <= '9') return c - '0' + 52;
    if (c == '+') return 62;
    if (c == '/') return 63;
    return 0; 
}

// decodes a single 3-byte instruction based on its instruction number
void decode_instruction(uint16_t instruction_number, uint8_t *decoded_bytes) {
    
    // Calculate the starting index in the encoded_data array
    uint32_t start_index = instruction_number * 4;

    // gets four Base64 characters from PROGMEM
    char char1 = pgm_read_byte(&encoded_data[start_index]);
    char char2 = pgm_read_byte(&encoded_data[start_index + 1]);
    char char3 = pgm_read_byte(&encoded_data[start_index + 2]);
    char char4 = pgm_read_byte(&encoded_data[start_index + 3]);

    // converting characters to 6-bit values
    uint8_t val1 = b64_char_to_val(char1);
    uint8_t val2 = b64_char_to_val(char2);
    uint8_t val3 = b64_char_to_val(char3);
    uint8_t val4 = b64_char_to_val(char4);

    // remake intothree bytes using bitwise shifts
    decoded_bytes[0] = (val1 << 2) | (val2 >> 4);
    decoded_bytes[1] = ((val2 & 0x0F) << 4) | (val3 >> 2);
    decoded_bytes[2] = ((val3 & 0x03) << 6) | val4;
}