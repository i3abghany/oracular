#include <kernel/console.h>
#include <kernel/kassert.h>
#include <kernel/plic.h>
#include <kernel/rv.h>
#include <kernel/trap.h>
#include <kernel/uart.h>
#include <kernel/virtio_blk.h>
#include <lib/stddef.h>

extern void trap_vec();

static isr_t isrs[N_IRQS];

void register_isr(uint32_t irq, isr_t isr)
{
    kassert(irq < N_IRQS);
    isrs[irq] = isr;
}

/*
 * Values taken from the RISC-V Privileged ISA Spec (sec. 4.1.9)
 */
static const char *scause_to_string(uint64_t scause)
{
    switch (scause) {
        case 0x8000000000000001:
            return "supervisor software interrupt";
        case 0x8000000000000005:
            return "supervisor timer interrupt";
        case 0x8000000000000009:
            return "supervisor external interrupt";
        case 0x0:
            return "instruction address misaligned";
        case 0x1:
            return "instruction access fault";
        case 0x2:
            return "illegal instruction";
        case 0x3:
            return "breakpoint";
        case 0x4:
            return "load address misaligned";
        case 0x5:
            return "load access fault";
        case 0x6:
            return "store/AMO address misaligned";
        case 0x7:
            return "store/AMO access fault";
        case 0x8:
            return "environment call from U-mode";
        case 0x9:
            return "environment call from S-mode";
        case 0xc:
            return "instruction page fault";
        case 0xd:
            return "load page fault";
        case 0xf:
            return "store/AMO page fault";
        default:
            return "invalid";
    }
}

void trap_init()
{
    set_stvec((uint64_t) trap_vec);
}

static inline uint8_t is_interrupt(uint64_t scause)
{
    return (scause & 0x8000000000000000) != 0;
}

void ktrap()
{
    uint64_t scause = get_scause();
    uint64_t trap_code = scause & 0xFF;
    if (is_interrupt(scause)) {
        if (trap_code == SUPERVISOR_EXTERNAL_INTERRUPT) {
            int irq = plic_claim();
            kassert(irq < N_IRQS);

            if (isrs[irq] == NULL) {
                panic("ktrap: Unknown PLIC interrupt: %d", irq);
            } else {
                isrs[irq]();
            }

            if (irq != 0) {
                plic_acknowledge(irq);
            }
        } else if (trap_code == SUPERVISOR_SOFTWARE_INTERRUPT) {
            kprintf("timer interrupt\n");
            /* Acknowledge the interrupt be resetting the pending bit. */
            set_sip(get_sip() & ~(1 << 1));
        }
    } else {
        panic("kernel exception\nscause: 0x%p, %s\nsepc: 0x%p\nstvec: 0x%p", scause,
              scause_to_string(scause), get_sepc(), get_stvec());
    }
}
