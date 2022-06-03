#include <kernel/list.h>

void intrusive_list_init(struct intrusive_list *l)
{
    l->next = l;
    l->prev = l;
}

static void intrusive_list_add_internal(struct intrusive_list *prev,
                                        struct intrusive_list *nn,
                                        struct intrusive_list *next)
{
    next->prev = nn;
    nn->next = next;
    nn->prev = prev;
    prev->next = nn;
}

void intrusive_list_append(struct intrusive_list *l, struct intrusive_list *nn)
{
    intrusive_list_add_internal(l, nn, l->next);
}

void intrusive_list_prepend(struct intrusive_list *l, struct intrusive_list *nn)
{
    intrusive_list_add_internal(l, nn, l->prev);
}

void intrusive_list_remove(struct intrusive_list *node)
{
    struct intrusive_list *next = node->next, *prev = node->prev;
    next->prev = prev;
    prev->next = next;
}
