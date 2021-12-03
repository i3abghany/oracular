#ifndef __KERNEL_CONSOLE_H
#define __KERNEL_CONSOLE_H

#include <kernel/uart.h>
#include <stdint.h>

int putchar(char ch);
int puts(const char *str);
uint8_t read_char();

#endif