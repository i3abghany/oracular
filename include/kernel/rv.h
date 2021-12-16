#ifndef __KERNEL_RV_H
#define __KERNEL_RV_H

#include <stdint.h>

#define CLINT_BASE     0x2000000
#define CLINT_MTIMECMP ((uint64_t *) (CLINT_BASE + 0x4000))
#define CLINT_MTIME    ((uint64_t *) (CLINT_BASE + 0xBFF8))

#define get_csr(x)                               \
    ({                                           \
        uint64_t ret;                            \
        asm volatile("csrr %0," #x : "=r"(ret)); \
        ret;                                     \
    })

#define set_csr(x, value) asm volatile("csrw " #x ", %0" : : "r"(value));

#endif
