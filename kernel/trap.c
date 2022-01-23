#include <kernel/console.h>
#include <kernel/rv.h>
#include <stdint.h>

extern void trap_vec();

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

void trap_init() { set_stvec((uint64_t) trap_vec); }

void ktrap()
{
    uint64_t scause = get_scause();

    /* Supervisor software interrupts are only triggered by timers. */
    if (scause & 1 && scause & 0x8000000000000000) {
        kprintf("timer interrupt\n");
        /* Acknowledge the interrupt be resetting the pending bit. */
        set_sip(get_sip() & ~(1 << 1));
    } else {
        panic("unknown kernel trap. (scause: 0x%p, %s)\n", scause,
              scause_to_string(scause));
    }
}
