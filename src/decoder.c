#include <stdint.h>     //inclueaded for specific integrer types and lengths
#include <stdio.h>      //included for input and outut functions

//follows the math of 

int main(void){ 

    //ALL posible values of base64, which are the values of ASCII characters
    static const  char b64_decoding_table[/*i dont remember how i started this*/] = {
        [0 ... (/*randomw*/ - 1)] = 64, 
        ['A'] = 0,  ['B'] = 1,  ['C'] = 2,  ['D'] = 3,
        ['E'] = 4,  ['F'] = 5,  ['G'] = 6,  ['H'] = 7,
        ['I'] = 8,  ['J'] = 9,  ['K'] = 10, ['L'] = 11,
        ['M'] = 12, ['N'] = 13, ['O'] = 14, ['P'] = 15,
        ['Q'] = 16, ['R'] = 17, ['S'] = 18, ['T'] = 19,
        ['U'] = 20, ['V'] = 21, ['W'] = 22, ['X'] = 23,
        ['Y'] = 24, ['Z'] = 25, ['a'] = 26, ['b'] = 27,
        ['c'] = 28, ['d'] = 29, ['e'] = 30, ['f'] = 31,
        ['g'] = 32, ['h'] = 33, ['i'] = 34, ['j'] = 35,
        ['k'] = 36, ['l'] = 37, ['m'] = 38, ['n'] = 39,
        ['o'] = 40, ['p'] = 41, ['q'] = 42, ['r'] = 43,
        ['s'] = 44, ['t'] = 45, ['u'] = 46, ['v'] = 47,
        ['w'] = 48, ['x'] = 49, ['y'] = 50, ['z'] = 51,
        ['0'] = 52, ['1'] = 53, ['2'] = 54, ['3'] = 55,
        ['4'] = 56, ['5'] = 57, ['6'] = 58, ['7'] = 59,
        ['8'] = 60, ['9'] = 61, ['+'] = 62, ['/'] = 63
    };

    //step 1 in decoding =>reads the encoded data and divide each character into a group (i say group because later on it will go on to become multiple digits) 
    //2 calls to the table value  and transforms each group into its numerical value, then reads that numerical as a binary
    //3the binary will outcome as an 8bit (byte) value so remove the 2 msb that will be 00
    //4 summ all the values into a single binary value

}
