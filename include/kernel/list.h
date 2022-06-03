#ifndef _KERNEL_LIST_H
#define _KERNEL_LIST_H

#include <stddef.h>

#define offset_of(base_type, field) ((size_t) & (((base_type *) 0)->field))

#define container_of(ptr, type, field) \
    ((type *) (((uint8_t *) ptr) - (size_t) offset_of(type, field)))

struct intrusive_list {
    struct intrusive_list *next;
    struct intrusive_list *prev;
};

void intrusive_list_init(struct intrusive_list *l);

void intrusive_list_append(struct intrusive_list *l, struct intrusive_list *nn);

void intrusive_list_prepend(struct intrusive_list *l, struct intrusive_list *nn);

void intrusive_list_remove(struct intrusive_list *node);

#define LIST_FOREACH(var, head) for (var = (head)->next; var != (head); var = var->next)

#endif
