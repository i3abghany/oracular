#ifndef _KERNEL_SPINLOCK_H
#define _KERNEL_SPINLOCK_H

#include <stdint.h>

struct spinlock_t {
    uint8_t acquired;
};

void spinlock_init(struct spinlock_t *s);
void acquire(struct spinlock_t *s);
void release(struct spinlock_t *s);

#endif
