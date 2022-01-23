#include <kernel/console.h>

static char digits[16] = "0123456789ABCDEF";

int putchar(int ch)
{
    uart0_put(ch);
    return ch;
}

int puts(const char *str)
{
    int i;

    for (i = 0; str[i] != '\0'; i++) {
        putchar(str[i]);
    }

    return i;
}

uint8_t read_char() { return uart0_get(); }

static inline uint8_t digit_to_ascii(uint8_t d, uint8_t base)
{
    if (base > 16) {
        panic("digit_to_ascii\n");
    }

    if (d < base) {
        return digits[d];
    } else {
        panic("digit_to_ascii\n");
    }
}

static void put_64bit_integer(uint64_t d, uint8_t base)
{
    if (d < base) {
        putchar(digit_to_ascii(d, base));
        return;
    }

    put_64bit_integer(d / base, base);
    put_64bit_integer(d % base, base);
}

static void putint(int d) { put_64bit_integer((uint64_t) d, 10); }

static void putptr(uint64_t d) { put_64bit_integer(d, 16); }

int kprintf(const char *fmt, ...)
{
    va_list ap;

    va_start(ap, fmt);
    kvprintf(fmt, ap);
    va_end(ap);
    return 0;
}

int kvprintf(const char *fmt, va_list args)
{
    for (uint32_t i = 0; fmt[i] != '\0'; i++) {
        if (fmt[i] == '%') {
            if (fmt[i + 1] == 'c') {
                char c = (char) va_arg(args, int);
                putchar(c);
                i++;
            } else if (fmt[i + 1] == 's') {
                const char *str = va_arg(args, const char *);
                puts(str);
                i++;
            } else if (fmt[i + 1] == 'd') {
                int d = va_arg(args, int);
                putint(d);
                i++;
            } else if (fmt[i + 1] == 'p') {
                uint64_t p = va_arg(args, uint64_t);
                putptr(p);
                i++;
            } else {
                putchar(fmt[i]);
            }
        } else {
            putchar(fmt[i]);
        }
    }

    return 0;
}
