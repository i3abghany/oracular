#include <kernel/console.h>
#include <stddef.h>
#include <stdint.h>

void kmain(void) {
    uart0_init();
    puts("Oracular Spectacular\n");

    while (1) {
        char c = read_char();
        if (c == 8) {
            putchar(8);
            putchar(' ');
            putchar(8);
        } else if (c == 10 || c == 13) {
            putchar('\n');
        } else {
            putchar(c);
        }
    }

    while (1)
        ;
}
