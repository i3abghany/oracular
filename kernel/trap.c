#include <kernel/console.h>
#include <stdint.h>

extern void trap_vec();

void trap_init() { set_stvec((uint64_t) trap_vec); }

void ktrap() {
    puts("PANIC: Inside ktrap.\n");
    while (1)
        ;
}
