#ifndef __KERNEL_RV_H
#define __KERNEL_RV_H

#include <stdint.h>

uint64_t get_mhartid();
uint64_t get_time();

void set_stvec(uint64_t addr);

#endif
