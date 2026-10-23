#ifndef UART_H
#define UART_H

#include <stdint.h>

void uart_putc(uint8_t c);
void stdio_init(void);
uint8_t uart_take_char(char *c);

#endif