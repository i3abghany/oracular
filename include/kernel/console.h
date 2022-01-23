#ifndef __KERNEL_CONSOLE_H
#define __KERNEL_CONSOLE_H

#include <kernel/uart.h>
#include <stdarg.h>
#include <stdint.h>

int putchar(int ch);
int puts(const char *str);
uint8_t read_char();
int kprintf(const char *fmt, ...);
int kvprintf(const char *fmt, va_list args);

#define panic(fmt, ...)                                   \
    do {                                                  \
        kprintf("panic @ (%s:%d): ", __FILE__, __LINE__); \
        kprintf(fmt, ##__VA_ARGS__);                      \
        for (;;) {                                        \
        }                                                 \
    } while (0)

#endif
