#include <kernel/console.h>
#include <kernel/kmem.h>
#include <kernel/slab_alloc.h>
#include <stddef.h>

struct slab_t *slab_init(const char *name, size_t obj_size)
{
    if (obj_size < sizeof(struct intrusive_list)) {
        kprintf(
            "slab_init: Slab allocation of objects of size less than a list pointer is "
            "not allowed.");
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

    struct intrusive_list *curr;
    for (size_t i = 0; i + ret->obj_size <= remaining_in_first_page; i += ret->obj_size) {
        curr = (struct intrusive_list *) ((uint64_t) start_addr + i);
        intrusive_list_append(&ret->empty_slabs, curr);
        ret->n_objects++;
    }
    ret->free = ret->n_objects;
    return ret;
}

void *slab_alloc(struct slab_t *slab)
{
    if (slab->free == 0) {
        /* TODO: get more memory. */
        kprintf("slab_alloc: out of memory\n");
        return NULL;
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
     * requirement of having every slab-allocated struct to have a list link
     * member explicitly. However, we now can only slab allocate objects of size
     * >= sizeof(struct intrusive_list)
     */
    intrusive_list_append(&slab->empty_slabs, (struct intrusive_list *) obj);
}
