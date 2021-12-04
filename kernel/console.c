#include <kernel/console.h>
#include <stdarg.h>

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

static inline uint8_t digit_to_ascii(uint8_t d) {
    if (d <= 9) {
        return d + '0';
    } else {
        return d;
    }
}

static void putint(int d) {
    if (d < 10) {
        putchar(digit_to_ascii(d));
        return;
    }

    putint(d / 10);
    putint(d % 10);
}

int kprintf(const char *fmt, ...) {
    va_list ap;

    va_start(ap, fmt);
    for (uint32_t i = 0; fmt[i] != '\0'; i++) {
        if (fmt[i] == '%') {
            if (fmt[i + 1] == 'c') {
                char c = (char) va_arg(ap, int);
                putchar(c);
                i++;
            } else if (fmt[i + 1] == 's') {
                const char *str = va_arg(ap, const char *);
                puts(str);
                i++;
            } else if (fmt[i + 1] == 'd') {
                int d = va_arg(ap, int);
                putint(d);
                i++;
            } else {
                putchar(fmt[i]);
            }
        } else {
            putchar(fmt[i]);
        }
    }
    va_end(ap);
    return 0;
}
