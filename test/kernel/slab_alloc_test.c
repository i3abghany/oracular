#include <kernel/console.h>
#include <kernel/kmem.h>
#include <kernel/slab_alloc.h>

#include "../test.h"

struct s1 {
    int i;
    char j;
    struct s1 *ptr1;
    struct s1 *ptr2;
    uint64_t a, b, c, d;
};

void test1()
{
    struct slab_t *s1_slab = slab_init("s1", sizeof(struct s1));
    uint64_t available_size = PAGE_SIZE - sizeof(struct slab_t);
    int n_can_allocate = available_size / sizeof(struct s1);
    int diff = n_can_allocate * sizeof(struct s1) - available_size;
    for (int i = 1; i <= n_can_allocate; i++) {
        struct s1 *sss = slab_alloc(s1_slab);
        ASSERT_NOT_NULL(sss);
        uint64_t expected_addr =
            (uint64_t) s1_slab + PAGE_SIZE - sizeof(struct s1) * i + diff;
        kprintf("%p, %p\n", sss, expected_addr);
        ASSERT_EQUAL(expected_addr, (uint64_t) sss);
    }
}
