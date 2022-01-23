#ifndef __KERNEL_KMEM_H
#define __KERNEL_KMEM_H

#define PAGE_SIZE 4096
#define PAGE_MASK 0xFFF

struct free_list_node {
    struct free_list_node *next;
};

void kmem_init();
void *page_alloc();
void page_free(void *page_addr);

#endif
