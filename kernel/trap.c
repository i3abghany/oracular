#include <kernel/console.h>
#include <stdint.h>

extern void trap_vec();

void trap_init() {
    asm volatile("csrw stvec, %0" : : "r" ((uint64_t)trap_vec));
}

void ktrap() {
    puts("PANIC: Inside ktrap.\n");
    while (1)
        ;
}
