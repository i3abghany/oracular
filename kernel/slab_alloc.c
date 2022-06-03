#include <kernel/console.h>
#include <kernel/kmem.h>
#include <kernel/slab_alloc.h>

struct slab_t *slab_init(const char *name, size_t obj_size)
{
    struct slab_t *ret = (struct slab_t *) page_alloc();
    ret->obj_size = obj_size;
    ret->name = name;
    ret->nobj = 0;
    ret->free = 0;
    intrusive_list_init(&ret->empty_slabs);

    size_t remaining_in_first_page = PAGE_SIZE - sizeof(struct slab_t);
    size_t start_addr = (uint64_t) ret + sizeof(struct slab_t);

    struct intrusive_list *curr;
    for (size_t i = 0; i + ret->obj_size <= remaining_in_first_page; i += ret->obj_size) {
        curr = (struct intrusive_list *) ((uint64_t) start_addr + i);
        intrusive_list_append(&ret->empty_slabs, curr);
        ret->nobj++;
    }
    ret->free = ret->nobj;
    return ret;
}

void *slab_alloc(struct slab_t *slab)
{
    if (slab->free == 0) {
        /* TODO: get more memory. */
        kprintf("slab_alloc: out of memory\n");
        return 0;
    }

    struct intrusive_list *elem;
    LIST_FOREACH(elem, &slab->empty_slabs)
    {
        slab->free--;
        intrusive_list_remove(elem);
        return (void *) elem;
    }

    panic("slab_alloc: no free place in the slab pool.");
    return 0;
}
