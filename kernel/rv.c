#include <kernel/rv.h>

uint64_t get_medeleg()
{
    uint64_t ret;
    asm volatile("csrr %0, medeleg" : "=r"(ret));
    return ret;
}

uint64_t get_mepc()
{
    uint64_t ret;
    asm volatile("csrr %0, mepc" : "=r"(ret));
    return ret;
}

uint64_t get_mhartid()
{
    uint64_t ret;
    asm volatile("csrr %0, mhartid" : "=r"(ret));
    return ret;
}

uint64_t get_mideleg()
{
    uint64_t ret;
    asm volatile("csrr %0, mideleg" : "=r"(ret));
    return ret;
}

uint64_t get_mie()
{
    uint64_t ret;
    asm volatile("csrr %0, mie" : "=r"(ret));
    return ret;
}

uint64_t get_mscratch()
{
    uint64_t ret;
    asm volatile("csrr %0, mscratch" : "=r"(ret));
    return ret;
}

uint64_t get_mstatus()
{
    uint64_t ret;
    asm volatile("csrr %0, mstatus" : "=r"(ret));
    return ret;
}

uint64_t get_scause()
{
    uint64_t ret;
    asm volatile("csrr %0, scause" : "=r"(ret));
    return ret;
}

uint64_t get_satp()
{
    uint64_t ret;
    asm volatile("csrr %0, satp" : "=r"(ret));
    return ret;
}

uint64_t get_sepc()
{
    uint64_t ret;
    asm volatile("csrr %0, sepc" : "=r"(ret));
    return ret;
}

uint64_t get_sstatus()
{
    uint64_t ret;
    asm volatile("csrr %0, sstatus" : "=r"(ret));
    return ret;
}

uint64_t get_stvec()
{
    uint64_t ret;
    asm volatile("csrr %0, stvec" : "=r"(ret));
    return ret;
}

uint64_t get_sie()
{
    uint64_t ret;
    asm volatile("csrr %0, sie" : "=r"(ret));
    return ret;
}

uint64_t get_sip()
{
    uint64_t ret;
    asm volatile("csrr %0, sip" : "=r"(ret));
    return ret;
}

uint64_t get_time()
{
    uint64_t ret;
    asm volatile("csrr %0, time" : "=r"(ret));
    return ret;
}

void set_medeleg(uint64_t value) { asm volatile("csrw medeleg, %0" : : "r"(value)); }
void set_mepc(uint64_t value) { asm volatile("csrw mepc, %0" : : "r"(value)); }
void set_mideleg(uint64_t value) { asm volatile("csrw mideleg, %0" : : "r"(value)); }
void set_mie(uint64_t value) { asm volatile("csrw mie, %0" : : "r"(value)); }
void set_mtvec(uint64_t value) { asm volatile("csrw mtvec, %0" : : "r"(value)); }
void set_mscratch(uint64_t value) { asm volatile("csrw mscratch, %0" : : "r"(value)); }
void set_mstatus(uint64_t value) { asm volatile("csrw mstatus, %0" : : "r"(value)); }
void set_sie(uint64_t value) { asm volatile("csrw sie, %0" : : "r"(value)); }
void set_sip(uint64_t value) { asm volatile("csrw sip, %0" : : "r"(value)); }
void set_sstatus(uint64_t value) { asm volatile("csrw sstatus, %0" : : "r"(value)); }
void set_stvec(uint64_t value) { asm volatile("csrw stvec, %0" : : "r"(value)); }
void set_pmpaddr0(uint64_t value) { asm volatile("csrw pmpaddr0, %0" : : "r"(value)); }
void set_pmpcfg0(uint64_t value) { asm volatile("csrw pmpcfg0, %0" : : "r"(value)); }
void set_satp(uint64_t value) { asm volatile("csrw satp, %0" : : "r"(value)); }
void set_scause(uint64_t value) { asm volatile("csrw scause, %0" : : "r"(value)); }

void qemu_virt_shutdown(enum shutdown_code code)
{
    switch (code) {
        case FAIL:
            *VIRT_MMIO_SHUTDOWN = VIRT_MMIO_FINISHER_FAIL;
            break;
        case PASS:
            *VIRT_MMIO_SHUTDOWN = VIRT_MMIO_FINISHER_PASS;
            break;
        default:
            while (1)
                ;
    }
}
