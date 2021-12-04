#include <kernel/rv.h>

uint64_t get_mhartid() {
    uint64_t ret;
    asm volatile("csrr %0, mhartid" : "=r"(ret));
    return ret;
}

uint64_t get_time() {
    uint64_t ret;
    asm volatile("csrr %0, time" : "=r"(ret));
    return ret;
}

void set_stvec(uint64_t addr) { asm volatile("csrw stvec, %0" : : "r"(addr)); }
