#include <avr/io.h>
#include <stdlib.h>
#include <stdio.h>
#include <avr/interrupt.h>
#include "uart.h"

#define RX_BUFFER_SIZE 64

// ring buffer structure
typedef struct {
    char data[RX_BUFFER_SIZE];
    uint8_t head;
    uint8_t tail;
} buffer_t;

static volatile buffer_t rx_buffer;

void uart_putc(uint8_t c) {
    while (!(USART0.STATUS & USART_DREIF_bm)); // Wait for TXDATA empty
    USART0.TXDATAL = c;
}//uart_putc

static int stdio_putchar(char c, FILE *stream) {
    uart_putc(c);
    return 0;
}
static FILE uart_stdout = FDEV_SETUP_STREAM(stdio_putchar, NULL, _FDEV_SETUP_WRITE);

void stdio_init(void) {
    stdout = &uart_stdout;
}

//read (fetches one character from the ring buffer if available)
uint8_t uart_take_char(char *c) {
    if (rx_buffer.head == rx_buffer.tail) {
        return 0; // Buffer empty
    }
    
    *c = rx_buffer.data[rx_buffer.tail];
    rx_buffer.tail = (rx_buffer.tail + 1) % RX_BUFFER_SIZE;
    
    return 1; // it works
}

//  ISR for RX
ISR(USART0_RXC_vect) {
    char received = USART0.RXDATAL;
    uint8_t next_head = (rx_buffer.head + 1) % RX_BUFFER_SIZE;
    
    // if the buffer is not full, then store the character
    if (next_head != rx_buffer.tail) {
        rx_buffer.data[rx_buffer.head] = received;
        rx_buffer.head = next_head;
    }
}