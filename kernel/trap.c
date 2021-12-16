#include <kernel/console.h>
#include <kernel/rv.h>
#include <stdint.h>

extern void trap_vec();

void trap_init() { set_csr(stvec, (uint64_t) trap_vec); }

void ktrap()
{
    uint64_t scause = get_csr(scause);

    /* Supervisor software interrupts are only triggered by timers. */
    if (scause & 1 && scause & 0x8000000000000000) {
        kprintf("timer interrupt\n");
        /* Acknowledge the interrupt be resetting the pending bit. */
        set_csr(sip, get_csr(sip) & ~(1 << 1));
    } else {
        kprintf("unknown kernel trap\n");
        while (1)
            ;
    }
}
