#include <kernel/rv.h>
#include <stdint.h>

extern void kmain();
extern void timer_vec();

uint64_t core_scratch[5];

void timer_init()
{
    uint64_t interrupt_interval = 10000000;
    uint64_t mtime = *CLINT_MTIME;
    *CLINT_MTIMECMP = mtime + interrupt_interval;

    core_scratch[3] = (uint64_t) CLINT_MTIMECMP;
    core_scratch[4] = interrupt_interval;
    set_csr(mscratch, (uint64_t) &core_scratch[0]);

    /* The only interrupt we handle in machine mode is timer interrupt. */
    set_csr(mtvec, (uint64_t) &timer_vec);

    /* Set MIE, globally enable machine mode interrupts. */
    set_csr(mstatus, get_csr(mstatus) | (1 << 3));

    /* Set MTIE, enable machine timer interrupt. */
    set_csr(mie, get_csr(mie) | (1 << 7));
}

static inline void delegate_events_to_supervisor()
{
    set_csr(medeleg, (uint64_t) 0xFFFF);
    set_csr(mideleg, (uint64_t) 0xFFFF);
    uint64_t sie_value = get_csr(sie);
    sie_value |= (1 << 9);  // SEIE (Exceptions enable)
    sie_value |= (1 << 5);  // STIE (Timer interrupts enable)
    sie_value |= (1 << 1);  // STIE (Software interrupts enable)
    set_csr(sie, sie_value);
}

__attribute__((noreturn)) void prekernel()
{
    /* Set previous privilege to supervisor mode for `mret` to change to it. */
    uint64_t mstatus_value = get_csr(mstatus);
    mstatus_value &= ~(3 << 11);
    mstatus_value |= (1 << 11);
    set_csr(mstatus, mstatus_value);

    /*
     * When a trap is taken into M-mode, mepc is written with the virtual
     * address of the instruction that was interrupted or that encountered the
     * exception. Otherwise, mepc is never written by the implementation, though
     * it may be explicitly written by software.
     *
     * Hence, we set `mepc` to `kmain` for `mret` to jump to it (in supervisor
     * mode)
     */
    set_csr(mepc, (uint64_t) &kmain);
    set_csr(satp, 0);
    delegate_events_to_supervisor();

    /*
     * Essentially disable physical memory protection to make all the memory
     * available for supervisor mode.
     */
    set_csr(pmpaddr0, 0x3FFFFFFFFFFFFF);

    /* Grant Read, Write, and Execute privileges. Indicate that the region is
     * Naturally aligned power-of-two region, >= 8bytes. */
    set_csr(pmpcfg0, 0x1F);

    timer_init();
    asm volatile("mret");

    /* This won't be reached. */
    while (1)
        ;
}
