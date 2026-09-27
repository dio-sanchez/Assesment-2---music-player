#include <avr/io.h>
#include <avr/interrupt.h>
#include "timer.h"


//NOT THE FINAL PRODUCT IM JUST USING THIS AS A TEMPLATE

ISR(PORTA_PORT_vect)
{
    if (PORTA.INTFLAGS & PORT_INT4_bm)//initial condition of s1 being pressed
     {
        while (PORTA.INTFLAGS & PORT_INT4_bm)
        {
            //while its pressed activate the potentiometer
        }
        //when the while loop stops e.g. the button is released, push the potentiometer value
        //activate the potentiometer
        PORTA.INTFLAGS = PORT_INT4_bm; // cclear
    }
    if (PORTA.INTFLAGS & PORT_INT5_bm) //s2 pressed
    {
        //if paused, apply next instruction while remaining paused
        //if on track selection work to decrease the selection
        PORTA.INTFLAGS = PORT_INT5_bm; 
    }
    if (PORTA.INTFLAGS & PORT_INT6_bm) //s3 pressed
     {
        //if state = track select,increase the track selection
        //else, go back to track_select
        PORTA.INTFLAGS = PORT_INT6_bm; 
    }
    if (PORTA.INTFLAGS & PORT_INT7_bm) // s4 pressed, 
     {
        /* 
        if playing, pasue
        if paused, play
        if selectring, start (play)
        */
        PORTA.INTFLAGS = PORT_INT7_bm; // cclear
    }
}