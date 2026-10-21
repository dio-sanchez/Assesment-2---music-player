#include <avr/io.h>
#include <stdlib.h>
#include <stdio.h>
#include <avr/interrupt.h>
#include <display.h>

/*     ABCDEFG
    0: 0000001
    1: 1001111
    2: 0010010
    3: 0000110
    4: 1001100
    5: 0100100
    6: 0100000
    7: 0001111
    8: 0000000
    9: 0000100
    A: 0001000
    B: 1100000
    C: 0110001
    D: 1000010
    E: 0110000
    F: 0111000
    */
// 7-segment display mapping 
// Bit order: E D C G B A F (Active-Low), this table was made using ai for rearranging all the values more efficiently
const uint8_t segment_map[16] = {
    0b0001000,  // 0: E=0, D=0, C=0, G=1, B=0, A=0, F=0
    0b1101011,  // 1: E=1, D=1, C=0, G=1, B=0, A=1, F=1
    0b0010001,  // 2: E=0, D=0, C=1, G=0, B=0, A=0, F=1
    0b1000001,  // 3: E=1, D=0, C=0, G=0, B=0, A=0, F=1
    0b1100010,  // 4: E=1, D=1, C=0, G=0, B=0, A=1, F=0
    0b1000100,  // 5: E=1, D=0, C=0, G=0, B=1, A=0, F=0
    0b0000100,  // 6: E=0, D=0, C=0, G=0, B=1, A=0, F=0
    0b1101001,  // 7: E=1, D=1, C=0, G=1, B=0, A=0, F=1
    0b0000000,  // 8: E=0, D=0, C=0, G=0, B=0, A=0, F=0
    0b1000000,  // 9: E=1, D=0, C=0, G=0, B=0, A=0, F=0
    0b0100000,  // A: E=0, D=1, C=0, G=0, B=0, A=0, F=0
    0b0000110,  // B: E=0, D=0, C=0, G=0, B=1, A=1, F=0
    0b0011100,  // C: E=0, D=0, C=1, G=1, B=1, A=0, F=0
    0b0000011,  // D: E=0, D=0, C=0, G=0, B=0, A=1, F=1
    0b0010100,  // E: E=0, D=0, C=1, G=0, B=1, A=0, F=0
    0b0110100   // F: E=0, D=1, C=1, G=0, B=1, A=0, F=0
};
volatile uint8_t left_byte = DISP_OFF | DISP_LHS;
volatile uint8_t right_byte = DISP_OFF;



ISR(SPI0_INT_vect)
{

    PORTA.OUTCLR = PIN1_bm; 

    PORTA.OUTSET = PIN1_bm; // latch high

    SPI0.INTFLAGS = SPI_IF_bm; // cli

}


void update_display(const uint8_t left, const uint8_t right)

{

    left_byte = (left & 0x7F) | DISP_LHS;
    right_byte = right & 0x7F;

}
