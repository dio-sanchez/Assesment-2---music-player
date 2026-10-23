#include <avr/io.h>
#include <stdlib.h>
#include <stdio.h>
#include <avr/interrupt.h>
#include <display.h>

volatile uint8_t spi_busy = 0;



// 7-segment display mapping
// Bit order: E D C G B A F (Active-Low), this table was made using ai for rearranging all the values more efficiently
const uint8_t segment_map[] = { 
    SEGS_ZERO, SEGS_ONE, SEGS_TWO, SEGS_THREE, SEGS_FOUR, SEGS_FIVE, SEGS_SIX, SEGS_SEVEN, 
    SEGS_EIGHT, SEGS_NINE, SEGS_A, SEGS_B, SEGS_C, SEGS_D, SEGS_E, SEGS_F 
};
volatile uint8_t left_byte = DISP_OFF | DISP_LHS;
volatile uint8_t right_byte = DISP_OFF;

//functions below taken from tutorial 10 demo

void find_hex_digits(uint8_t num, uint8_t* digit1, uint8_t* digit0) {
    *digit1 = (num >> 4) & 0x0F;
    *digit0 = (num & 0x0F);

    left_byte = segment_map[*digit1];
    right_byte = segment_map[*digit0];
}//find_dec_digits

void set_display_segments(uint8_t segs_l, uint8_t segs_r) {
    left_byte = segs_l;
    right_byte = segs_r;
}//set_display_segments


void swap_display_digit(void) {
    static int digit = 0;
    if (digit) {
        display_write(left_byte | (0x01 << 7));
    } else {
        display_write(right_byte);
    }
    digit = !digit;
}//swap_digit

void display_write(uint8_t data) {
    PORTA.OUTCLR = PIN1_bm;   // latch low
    spi_busy = 1;
    SPI0.DATA = data;
}//display_write


ISR(SPI0_INT_vect)
{
    PORTA.OUTSET = PIN1_bm;   // latch high
    SPI0.INTFLAGS = SPI_IF_bm;
    spi_busy = 0;
}