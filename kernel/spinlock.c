#include <kernel/console.h>
#include <kernel/spinlock.h>

void spinlock_init(struct spinlock_t *s) { s->acquired = 0; }

void acquire(struct spinlock_t *s)
{
    if (s->acquired) {
        panic("acquire: attempting to acquire an already-acquired lock.");
    }

    while (__atomic_test_and_set(&s->acquired, __ATOMIC_SEQ_CST) != 0)
        ;
}

void release(struct spinlock_t *s)
{
    if (!s->acquired) {
        panic("release: attempting to release a lock that isn't acquired.");
    }

    __atomic_clear(&s->acquired, __ATOMIC_SEQ_CST);
}
