#include <kernel/console.h>
#include <kernel/kalloc.h>
#include <kernel/rv.h>
#include <stdint.h>

void kmain(void)
{
    uart0_init();
    puts("Oracular Spectacular\n");

    void trap_init();
    trap_init();

    kmem_init();

    /* Enable supervisor software interrupts. */
    set_csr(sstatus, get_csr(sstatus) | (1 << 1));

    while (1) {
        char c = read_char();
        if (c == 0x7f) {
            putchar(8);
            putchar(' ');
            putchar(8);
        } else if (c == 10 || c == 13) {
            putchar('\n');
        } else {
            putchar(c);
        }
    }
}
