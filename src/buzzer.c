#include <stdint.h> //inclueaded for specific integrer types and lengths
#include <stdio.h>  //included for input and outut functions
#include <avr/interrupt.h>
#include "buzzer.h"

void buzzer_stop(void)
{
    TCA0.SINGLE.CTRLB &= ~TCA_SINGLE_CMP0EN_bm;
    TCA0.SINGLE.CMP0 = 0;
    PORTB.OUTCLR = PIN0_bm;
}
void buzzer_play_freq(uint16_t freq)
{
    if (freq == 0)
    {
        buzzer_stop();
        return;
    }

    // Calculate period for the given frequency (assuming 3.33MHz / 2 prescaler)
    uint16_t period = (3333333UL / 2 / freq) - 1;

    TCA0.SINGLE.PER = period;
    TCA0.SINGLE.CMP0 = period / 2;             // 50% duty cycle for square wave
    TCA0.SINGLE.CMP1 = period;                 // supposed to maintain the brightess as a maximum
    TCA0.SINGLE.CTRLB |= TCA_SINGLE_CMP0EN_bm; // Enable output
}
