#ifndef __KERNEL_RV_H
#define __KERNEL_RV_H

#include <stdint.h>

#define CLINT_BASE     0x2000000
#define CLINT_MTIMECMP ((uint64_t *) (CLINT_BASE + 0x4000))
#define CLINT_MTIME    ((uint64_t *) (CLINT_BASE + 0xBFF8))

uint64_t get_medeleg();
uint64_t get_mepc();
uint64_t get_mhartid();
uint64_t get_mideleg();
uint64_t get_mie();
uint64_t get_mscratch();
uint64_t get_mstatus();
uint64_t get_sstatus();
uint64_t get_scause();
uint64_t get_sie();
uint64_t get_sip();
uint64_t get_time();

void set_medeleg(uint64_t value);
void set_mepc(uint64_t value);
void set_mideleg(uint64_t value);
void set_mie(uint64_t value);
void set_mtvec(uint64_t value);
void set_mscratch(uint64_t value);
void set_mstatus(uint64_t value);
void set_sie(uint64_t value);
void set_sip(uint64_t value);
void set_stvec(uint64_t value);
void set_pmpaddr0(uint64_t value);
void set_pmpcfg0(uint64_t value);
void set_satp(uint64_t value);
void set_sstatus();

#endif
