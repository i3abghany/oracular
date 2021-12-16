#ifndef __KERNEL_KALLOC_H
#define __KERNEL_KALLOC_H

#define PAGE_SIZE 4096

struct free_list_node {
    struct free_list_node *next;
};

void kmem_init();
void *page_alloc();
void page_free(void *page_addr);

#endif
