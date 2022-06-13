#ifndef _KERNEL_SPINLOCK_H
#define _KERNEL_SPINLOCK_H

#include <stdint.h>

struct spinlock_t {
    uint8_t acquired;
    const char *name;
};

void spinlock_init(struct spinlock_t *s, const char *name);
void spinlock_acquire(struct spinlock_t *s);
void spinlock_release(struct spinlock_t *s);

#endif
