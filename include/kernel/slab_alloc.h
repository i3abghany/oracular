#ifndef _KERNEL_SLAB_ALLOC_H
#define _KERNEL_SLAB_ALLOC_H

#include <kernel/list.h>
#include <stdint.h>

static struct intrusive_list slabs_list = {&slabs_list, &slabs_list};

struct slab_t {
    size_t obj_size;
    uint64_t nobj;
    uint64_t free;
    const char *name;
    struct intrusive_list empty_slabs;
};

struct slab_t *slab_init(const char *name, size_t obj_size);

void *slab_alloc(struct slab_t *slab);

void slab_free(struct slab_t *slab, void *obj);

#endif
