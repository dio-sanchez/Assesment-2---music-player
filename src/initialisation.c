#include <avr/io.h>
#include <stdlib.h>
#include <stdio.h>
#include <avr/interrupt.h>
#include "initialisation.h"

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

//global variable that holds the current state
//first initialised in the track selection state
State_t State = TRACK_SELECT;


//initialises the programs with all the buttons set to pull up resistors and falling edge interrupts
void buttons_init(void) {
    PORTA.PIN4CTRL |= PORT_PULLUPEN_bm | PORT_ISC_FALLING_gc;
    PORTA.PIN5CTRL |= PORT_PULLUPEN_bm | PORT_ISC_FALLING_gc;
    PORTA.PIN6CTRL |= PORT_PULLUPEN_bm | PORT_ISC_FALLING_gc;
    PORTA.PIN7CTRL |= PORT_PULLUPEN_bm | PORT_ISC_FALLING_gc;
}


void display_init(void) {
    PORTMUX.SPIROUTEA = PORTMUX_SPI0_ALT1_gc;  // SPI pins on PC0-3

    // SPI SCK and MOSI
    PORTC.DIRSET = (PIN0_bm | PIN2_bm);   // SCK (PC0) and MOSI (PC2) output

    // DISP_LATCH
    PORTA.OUTSET = PIN1_bm;        // DISP_LATCH initial high
    PORTA.DIRSET = PIN1_bm;        // set DISP_LATCH pin as output

    SPI0.CTRLA = SPI_MASTER_bm;    // Master, /4 prescaler, MSB first
    SPI0.CTRLB = SPI_SSD_bm;       // Mode 0, client select disable, unbuffered
    SPI0.INTCTRL = SPI_IE_bm;      // Interrupt enable
    SPI0.CTRLA |= SPI_ENABLE_bm;   // Enable
}


void timer_init(void) {

    // configure TCB1 for a periodic interrupt every 5ms
    TCB1.CTRLB = TCB_CNTMODE_INT_gc;    // Configure TCB1 in periodic interrupt mode
    TCB1.CCMP = 16667;                  // Set interval for 5ms (16667 clocks @ 3.3 MHz)
    TCB1.INTCTRL = TCB_CAPT_bm;         // CAPT interrupt enable
    TCB1.CTRLA = TCB_ENABLE_bm;         // Enable TCB1
}


void uart_init(void) {

    PORTB.DIRSET = PIN2_bm;    //enable PB2 as an output

    USART0.BAUD = 1389;        // 9600 BAUD @ 3.33 MHz
//    USART0.CTRLA = USART_RXCIE_bm | USART_DREIE_bm;   // enable DRE / RX interrupts
    USART0.CTRLB = USART_RXEN_bm | USART_TXEN_bm;  //enable Tx/Rx
}//uart_init

void adc_init(void)
{
    ADC0.CTRLA = ADC_ENABLE_bm;
    ADC0.CTRLB = ADC_PRESC_DIV2_gc;
    ADC0.CTRLC = (4 << ADC_TIMEBASE_gp) | ADC_REFSEL_VDD_gc;
    ADC0.CTRLE = 64;
    ADC0.CTRLF = ADC_FREERUN_bm | ADC_LEFTADJ_bm;
    ADC0.MUXPOS = ADC_MUXPOS_AIN2_gc;
    ADC0.COMMAND = ADC_MODE_SINGLE_8BIT_gc | ADC_START_IMMEDIATE_gc;
} //potentiometer


void pwm_init(void)
{
    // SETS the disp as an output
    PORTB.OUTSET = PIN1_bm;
    PORTB.DIRSET = PIN1_bm;
    PORTB.OUTSET = PIN0_bm;
    PORTB.DIRSET = PIN0_bm;

    TCA0.SINGLE.CTRLA = TCA_SINGLE_CLKSEL_DIV2_gc;
    TCA0.SINGLE.CTRLB = TCA_SINGLE_WGMODE_SINGLESLOPE_gc | TCA_SINGLE_CMP0EN_bm | TCA_SINGLE_CMP1EN_bm;

    TCA0.SINGLE.PER = 10416 / 2;

    // led brightness percentage
    TCA0.SINGLE.CMP1 = 1145 / 2; // 11%

    TCA0.SINGLE.CTRLA |= 0x01;
}