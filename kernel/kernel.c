#include <stddef.h>
#include <stdint.h>

#include "uart.h"

int putchar(char ch) {
    uart0_putc(ch);
    return ch;
}

int puts(const char *str) {
    int i;

    for (i = 0; str[i] != '\0'; i++) {
        putchar(str[i]);
    }

    return i;
}

void kmain(void) {
    uart0_init();
    puts("Oracular Spectacular\n");
    while (1)
        ;
}
