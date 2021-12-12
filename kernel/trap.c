#include <kernel/console.h>
#include <kernel/rv.h>
#include <stdint.h>

extern void trap_vec();

void trap_init() { set_stvec((uint64_t) trap_vec); }

void ktrap() {
    uint64_t a1_value = 0;
    asm volatile("add %0, a1, zero" : "=r"(a1_value));

    if (a1_value == 69) {
        kprintf("TIMER INTERRUPT\n");
        return;
    } else {
        kprintf("PANIC: in ktrap\n");
    }

    while (1)
        ;
}

