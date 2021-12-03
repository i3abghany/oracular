#include <kernel/console.h>

int putchar(int ch) {
    uart0_put(ch);
    return ch;
}

int puts(const char *str) {
    int i;

    for (i = 0; str[i] != '\0'; i++) {
        putchar(str[i]);
    }

    return i;
}

uint8_t read_char() { return uart0_get(); }
