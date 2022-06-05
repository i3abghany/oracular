#include <kernel/console.h>
#include <kernel/kmem.h>
#include <kernel/slab_alloc.h>
#include <lib/stddef.h>

static void slab_add_mem(struct slab_t *slab, uint64_t start_addr, size_t size)
{
    struct intrusive_list *curr;
    for (size_t i = 0; i + slab->obj_size <= size; i += slab->obj_size) {
        curr = (struct intrusive_list *) ((uint64_t) start_addr + i);
        intrusive_list_append(&slab->empty_slabs, curr);
        slab->n_objects++;
        slab->free++;
    }
}

struct slab_t *slab_init(const char *name, size_t obj_size)
{
    if (obj_size < sizeof(struct intrusive_list)) {
        kprintf(
            "slab_init: Slab allocation of objects of size less than a list pointer is "
            "not allowed.\n");
        return NULL;
    }
    struct slab_t *ret = (struct slab_t *) page_alloc();
    ret->obj_size = obj_size;
    ret->name = name;
    ret->n_objects = 0;
    ret->free = 0;
    intrusive_list_init(&ret->empty_slabs);

    size_t remaining_in_first_page = PAGE_SIZE - sizeof(struct slab_t);
    size_t start_addr = (uint64_t) ret + sizeof(struct slab_t);

    slab_add_mem(ret, start_addr, remaining_in_first_page);
    return ret;
}

void *slab_alloc(struct slab_t *slab)
{
    if (slab->free == 0) {
        uint64_t new_page_addr = (uint64_t) page_alloc();
        slab_add_mem(slab, new_page_addr, PAGE_SIZE);
    }

    struct intrusive_list *elem;
    LIST_FOREACH(elem, &slab->empty_slabs)
    {
        slab->free--;
        intrusive_list_remove(elem);
        return (void *) elem;
    }

    panic("slab_alloc: no free space in the slab pool.");
    return NULL;
}

void slab_free(struct slab_t *slab, void *obj)
{
    slab->free++;
    /*
     * The first sizeof(struct intrusive_list) bytes of "obj" is used as a link
     * for the entry to be appended in the linked list. This alleviates the
     * requirement of having every slab-allocated struct have a list link member
     * explicitly. However, we now can only slab allocate objects of size >=
     * sizeof(struct intrusive_list)
     */
    intrusive_list_append(&slab->empty_slabs, (struct intrusive_list *) obj);
}
