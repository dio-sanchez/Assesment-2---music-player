#include <stdint.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>


#include "decoder.h"
#include "data.h"
#include "globals.h"
#include "timer.h"
#include "buzzer.h"

#include "display.h"

const uint16_t frequency_table[12] = {
    262, // 0x0: C
    277, // 0x1: C#
    294, // 0x2: D
    311, // 0x3: D#
    330, // 0x4: E
    349, // 0x5: F
    370, // 0x6: F#
    392, // 0x7: G
    415, // 0x8: G#
    440, // 0x9: A  (Base frequency)
    466, // 0xA: A#
    494  // 0xB: B
};

// function to map ase64 characters and corresponding decoded values.
uint8_t base64_decode_char(char c)
{
    if (c >= 'A' && c <= 'Z')
    {
        return c - 'A';
    }
    else if (c >= 'a' && c <= 'z')
    {
        return c - 'a' + 26;
    }
    else if (c >= '0' && c <= '9')
    {
        return c - '0' + 52;
    }
    else if (c == '+')
    {
        return 62;
    }
    else if (c == '/')
    {
        return 63;
    }
    return 0;
}

// decodes a single 3-byte instruction based on its instruction number
void decode_block(const char *input_ptr, uint8_t *out_bytes)
{
    uint8_t c1 = base64_decode_char(input_ptr[0]);
    uint8_t c2 = base64_decode_char(input_ptr[1]);
    uint8_t c3 = base64_decode_char(input_ptr[2]);
    uint8_t c4 = base64_decode_char(input_ptr[3]);
    out_bytes[0] = (c1 << 2) | (c2 >> 4);
    out_bytes[1] = ((c2 & 0x0F) << 4) | (c3 >> 2);
    out_bytes[2] = ((c3 & 0x03) << 6) | c4;
}

typedef struct
{
    uint16_t period;      // TCA0.SINGLE.PER value for note pitch
    uint16_t duration_ms; // Note play time in milliseconds
} Note;

Note note_parser(uint8_t byte1, uint8_t byte2)
{
    Note note;
    uint8_t pitch_index = byte1;
    note.duration_ms = byte2 * 10; // 10ms units

    if (pitch_index == 0)
    {
        note.period = 0; // Rest / Silence
    }
    else
    {
        // period based of T = 1/f, with 1s being 3.33hz and f being , well f[whatever pitch]
        note.period = (3333333UL / (2UL * frequency_table[pitch_index])) - 1;
    }
    return note;
}

// lfsr algorythm
uint8_t descramble_byte(uint8_t decoded_byte, uint32_t *lfsr)
{

    uint8_t bit = *lfsr & 1;
    *lfsr >>= 1;
    if (bit == 1)
    {
        *lfsr ^= 0xE2026E6B; // constant lfsr mask
    }
    uint8_t ls_byte = (uint8_t)(*lfsr & 0xFF);
    return decoded_byte ^ ls_byte;
}

uint16_t calculate_frequency(uint8_t octave, uint8_t note)
{
    if (octave == 0 || note > 0xB)
        return 0;

    uint16_t base_freq = frequency_table[note];

    if (octave >= 4)
    {
        return base_freq << (octave - 4);
    }
    else
    {
        return base_freq >> (4 - octave);
    }
}

extern volatile uint8_t spi_busy;   // from the display latch fix (declare in display.h)

// Drives buzzer + display for one decoded instruction.
// Returns 1 if it was a terminating instruction (nothing is played), else 0.



// Starts the countdown for the instruction just applied (PLAYING / resume).

void fetch_instruction(uint16_t index, uint8_t track4, uint32_t *lfsr, uint8_t ins[3])
{
    char b64[4];
    memcpy_P(b64, &encoded_data[index], 4);
    decode_block(b64, ins);

    if (track4)
    {
        for (uint8_t i = 0; i < 3; i++)
            ins[i] = descramble_byte(ins[i], lfsr);
    }
}

// Drives the buzzer and display for one instruction.
// Returns 1 if it was a terminator (nothing is played), otherwise 0.
uint8_t execute_instruction(const uint8_t ins[3], uint8_t multiplier, uint16_t *full_duration)
{
    if (ins[0] == 0 && ins[1] == 0 && ins[2] == 0)
        return 1;

    buzzer_play_freq(calculate_frequency(ins[1] >> 4, ins[1] & 0x0F));
    *full_duration = (uint16_t)ins[0] * multiplier;

    while (spi_busy);
    display_write(ins[2]);
    return 0;
}
