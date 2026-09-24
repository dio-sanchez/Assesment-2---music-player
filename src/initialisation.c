#include <avr/io.h>
#include <stdlib.h>
#include <stdio.h>
#include <avr/interrupt.h>
#include "initialisation.h"


//global variable that holds the current state
//first initialised in the track selection state
State_t State = TRACK_SELECT;


//initialises the programs with all the buttons set to pull up resistors and falling edge interrupts
void init_ports(void) {
    PORTA.PIN4CTRL |= PORT_PULLUPEN_bm | PORT_ISC_FALLING_gc;
    PORTA.PIN5CTRL |= PORT_PULLUPEN_bm | PORT_ISC_FALLING_gc;
    PORTA.PIN6CTRL |= PORT_PULLUPEN_bm | PORT_ISC_FALLING_gc;
    PORTA.PIN7CTRL |= PORT_PULLUPEN_bm | PORT_ISC_FALLING_gc;
}
void init_display(void) {
    PORTA.PIN1CTRL |= PORT_PULLUPEN_bm; //set pin 1 as output for the display
    PORTC.PIN0CTRL |= PORT_PULLUPEN_bm;
    PORTC.PIN2CTRL |= PORT_PULLUPEN_bm;
}



//sets the event when a buttons is pressed, any change in the state is handled here
void handle_event(Events event) {
    switch (State) {
        case TRACK_SELECT:
            switch (event) {
                case S4_PRESSED:
                    State = PLAYING;
                    break;
                default:
                    exit(1);
                    break;
            }
            break;

        case PLAYING:
            switch (event) {
                case S4_PRESSED:
                    State = PAUSED;
                    break;
                case S3_PRESSED:
                    State = TRACK_SELECT;
                    break;
                default:
                    exit(1);
                    break;
            }
            break;

        case PAUSED:
            switch (event) {
                case S4_PRESSED:
                    State = PLAYING;
                    break;
                case S3_PRESSED:
                    State = TRACK_SELECT;
                    break;
                default:
                    exit(1);
                    break;
            }
            break;
    }
}
