#ifndef __KERNEL_KASSERT_H
#define __KERNEL_KASSERT_H

#define kassert(exp)                               \
    do {                                           \
        if (!(exp)) {                              \
            panic("kassert: (%s) failed\n", #exp); \
        }                                          \
    } while (0)

#endif
