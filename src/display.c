#include <avr/io.h>
#include <stdlib.h>
#include <stdio.h>
#include <avr/interrupt.h>
#include <display.h>

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
