#ifndef DISPLAY_H
#define DISPLAY_H

/*
Decimal             dp marks the digit so it is irrelevant
Digit	Individual Segments Illuminated
    Q5  Q4  Q2  Q1  Q0  Q6  Q3  Q7
    a	b	c	d	e	f	g   
0	×	×	×	×	×	×	    - 
1	 	×	×	 	 	 	 
2	×	×	 	×	×	 	×
3	×	×	×	×	 	 	×
4	 	×	×	 	 	×	×
5	×	 	×	×	 	×	×
6	×	 	×	×	×	×	×
7	×	×	×	 	 	 	 
8	×	×	×	×	×	×	×
9	×	×	×	 	 	×	×



*/
#define a   0b00000100            //Segment A is connected to Q5
#define b   0b00001000            //Segment B is connected to Q4
#define c   0b00100000            //Segment C is connected to Q2
#define d   0b01000000            //Segment D is connected to Q1
#define e   0b10000000            //Segment E is connected to Q0
#define f   0b00000010            //Segment F is connected to Q6
#define g   0b00010000            //Segment G is connected to Q3
#define DP  0b00000001            //Segment DP is connected to Q7

//all possivle segment patterns
typedef enum digits {
    one,
    two,
    three,
    four,
    five,
    six,
    seven,
    eight,
    nine,
    zero,
    A,
    B,
    C,
    D,
    E,
    F

} digits;

//either the left digit or the right digit
typedef enum position{
    left,
    right
} position;

#endif

