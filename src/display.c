#include <stdint.h>
#include <stdio.h>
#include <display.h>

const uint8_t digit_gen[16] =        //Digits generator    
{
            ~(a+b+c+d+e+f),         //0
            ~(b+c),                 //1
            ~(a+b+d+e+g),           //2
            ~(a+b+c+d+g),           //3
            ~(f+g+b+c),             //4
            ~(a+f+g+c+d),           //5
            ~(a+f+e+d+c+g),         //6
            ~(a+b+c),               //7
            ~(a+b+c+d+e+f+g),       //8
            ~(d+c+b+a+f+g),         //9
            ~(a+b+c+e+f+g),         //A
            ~(c+d+e+f+g),           //B
            ~(a+d+e+f),             //C
            ~(b+c+d+e+g),           //D
            ~(a+d+e+f+g),           //E
            ~(a+e+f+g)              //F
};
