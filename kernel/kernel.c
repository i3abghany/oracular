#include <kernel/console.h>
#include <kernel/kalloc.h>
#include <kernel/rv.h>
#include <kernel/vm.h>
#include <stdint.h>

void kmain(void)
{
    uart0_init();
    puts("Oracular Spectacular\n");

    kprintf("enabling traps...");
    void trap_init();
    trap_init();
    kprintf("done\n");

    kprintf("initializing kernel memory...");
    kmem_init();
    kprintf("done\n");

    kvm_init();

    kprintf("enabling supervisor software interrupts...");
    set_sstatus(get_sstatus() | (1 << 1));
    kprintf("done\n");

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
