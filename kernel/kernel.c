#include <stddef.h>
#include <stdint.h>

uint8_t *uart = (uint8_t *) 0x10000000;

#define UART                0x10000000
#define UART_THR            (uint8_t *)(UART + 0x00)
#define UART_LSR            (uint8_t *)(UART + 0x05)
#define UART_LSR_EMPTY_MASK 0x40

int putchar(char ch) {
	while ((*UART_LSR & UART_LSR_EMPTY_MASK) == 0);
	return *UART_THR = ch;
}

int puts(const char *str) {
    int i;

    for (i = 0; str[i] != '\0'; i++) {
        putchar(str[i]);
    }

    return i;
}

void kmain(void) {
    puts("Oracular Spectacular\n");
    while (1);
}
