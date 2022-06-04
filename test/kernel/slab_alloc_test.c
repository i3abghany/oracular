#include <kernel/console.h>
#include <kernel/kmem.h>
#include <kernel/slab_alloc.h>

#include "../test.h"

struct s1 {
    // struct intrusive_list list_head;
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
        ASSERT_EQUAL(expected_addr, (uint64_t) sss);
    }
}

void test2()
{
    struct slab_t *s1_slab = slab_init("s1", sizeof(struct s1));
    struct s1 *sss1 = slab_alloc(s1_slab);
    slab_free(s1_slab, (void *) sss1);
    struct s1 *sss2 = slab_alloc(s1_slab);
    ASSERT_EQUAL(sss1, sss2);

    struct s1 *sss3 = slab_alloc(s1_slab);
    struct s1 *sss4 = slab_alloc(s1_slab);
    slab_free(s1_slab, sss3);
    slab_free(s1_slab, sss4);
    struct s1 *sss5 = slab_alloc(s1_slab);
    struct s1 *sss6 = slab_alloc(s1_slab);

    ASSERT_EQUAL(sss3, sss6);
    ASSERT_EQUAL(sss4, sss5);
}

void slab_alloc_tests()
{
    test1();
    kprintf("slab_alloc_tests: test1 [pass]\n");

    test2();
    kprintf("slab_alloc_tests: test2 [pass]\n");
}
