#include <kernel/console.h>
#include <kernel/spinlock.h>

void spinlock_init(struct spinlock_t *s, const char *name)
{
    s->acquired = 0;
    s->name = name;
}

void spinlock_acquire(struct spinlock_t *s)
{
    if (s->acquired) {
        panic("acquire: Attempting to acquire an already-acquired lock. (%s)", s->name);
    }

    while (__atomic_test_and_set(&s->acquired, __ATOMIC_SEQ_CST) != 0)
        ;
}

void spinlock_release(struct spinlock_t *s)
{
    if (!s->acquired) {
        panic("release: attempting to release a lock that isn't acquired. (%s)", s->name);
    }

    __atomic_clear(&s->acquired, __ATOMIC_SEQ_CST);
}
