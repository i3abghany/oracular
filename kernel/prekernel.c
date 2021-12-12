#include <kernel/rv.h>
#include <stdint.h>

extern void kmain();
extern void timer_vec();

uint64_t core_scratch[5];

void timer_init() {
    uint64_t interrupt_interval = 1000000;
    uint64_t mtime = *CLINT_MTIME;
    *CLINT_MTIMECMP = mtime + interrupt_interval;

    core_scratch[3] = (uint64_t) CLINT_MTIMECMP;
    core_scratch[4] = interrupt_interval;
    set_mscratch((uint64_t) &core_scratch[0]);

    set_mtvec((uint64_t) &timer_vec);
    set_mstatus(get_mstatus() | (1 << 3));

    set_mie(get_mie() | (1 << 7));
}

static inline void delegate_events_to_supervisor() {
    set_medeleg((uint64_t) 0xFFFF);
    set_mideleg((uint64_t) 0xFFFF);
    uint64_t sie = get_sie();
    sie |= (1 << 9);    // SEIE (Exceptions enable)
    sie |= (1 << 5);    // STIE (Timer interrupts enable)
    sie |= (1 << 1);    // STIE (Software interrupts enable)
    set_sie(sie);
}

void prekernel() {
    /* Set previous privilege to supervisor mode for `mret` to change to it. */
    uint64_t mstatus = get_mstatus();
    mstatus &= ~(3 << 11);
    mstatus |= (1 << 11);
    set_mstatus(mstatus);

    /*
     * When a trap is taken into M-mode, mepc is written with the virtual
     * address of the instruction that was interrupted or that encountered the
     * exception. Otherwise, mepc is never written by the implementation, though
     * it may be explicitly written by software.
     *
     * Hence, we set `mepc` to `kmain` for `mret` to jump to it (in supervisor
     * mode)
     */
    set_mepc((uint64_t) &kmain);
    delegate_events_to_supervisor();
    timer_init();

    asm volatile("mret");
}
