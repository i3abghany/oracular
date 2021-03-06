#ifndef _KERNEL_RV_H
#define _KERNEL_RV_H

#include <stdint.h>

#define REG_ZERO 0
#define REG_RA   1
#define REG_SP   2
#define REG_GP   3
#define REG_TP   4
#define REG_T0   5
#define REG_T1   6
#define REG_T2   7
#define REG_S0   8
#define REG_S1   9
#define REG_A0   10
#define REG_A1   11
#define REG_A2   12
#define REG_A3   13
#define REG_A4   14
#define REG_A5   15
#define REG_A6   16
#define REG_A7   17
#define REG_S2   18
#define REG_S3   19
#define REG_S4   20
#define REG_S5   21
#define REG_S6   22
#define REG_S7   23
#define REG_S8   24
#define REG_S9   25
#define REG_S10  26
#define REG_S11  27
#define REG_T3   28
#define REG_T4   29
#define REG_T5   30
#define REG_T6   31

#define CLINT_BASE     0x2000000
#define CLINT_MTIMECMP ((uint64_t *) (CLINT_BASE + 0x4000))
#define CLINT_MTIME    ((uint64_t *) (CLINT_BASE + 0xBFF8))

#define VIRT_MMIO_BASE     0x100000
#define VIRT_MMIO_SHUTDOWN ((uint16_t *) (VIRT_MMIO_BASE + 0))

#define VIRT_MMIO_FINISHER_FAIL 0x3333
#define VIRT_MMIO_FINISHER_PASS 0x5555

uint64_t get_medeleg();
uint64_t get_mepc();
uint64_t get_mhartid();
uint64_t get_mideleg();
uint64_t get_mie();
uint64_t get_mscratch();
uint64_t get_mstatus();
uint64_t get_satp();
uint64_t get_sepc();
uint64_t get_sstatus();
uint64_t get_stvec();
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

enum shutdown_code { PASS, FAIL };

void qemu_virt_shutdown(enum shutdown_code code);

#endif
