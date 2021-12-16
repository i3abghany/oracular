#include <kernel/console.h>
#include <kernel/kalloc.h>
#include <stddef.h>
#include <stdint.h>

struct free_list_node *page_list;

extern char _kernel_mem_end[];
extern char _physical_mem_end[];

static uint64_t KMEM_END;
static uint64_t MEM_TOP;

static void *memset(void *dest, int c, size_t size)
{
    uint8_t *d = (uint8_t *) dest;

    for (size_t i = 0; i < size; i++) {
        d[i] = c;
    }

    return dest;
}

void kmem_init()
{
#ifdef KERNEL_DEBUG
    kprintf("kmem_end: %p\n", (uint64_t) _kernel_mem_end);
    kprintf("phys_top: %p\n", (uint64_t) _physical_mem_end);
#endif

    KMEM_END = (uint64_t) _kernel_mem_end;
    MEM_TOP = (uint64_t) _physical_mem_end;

    for (uint64_t page_addr = KMEM_END; page_addr < MEM_TOP; page_addr += PAGE_SIZE)
        page_free((void *) page_addr);
}

void *page_alloc()
{
    struct free_list_node *node = page_list;
    if (node == NULL) {
        kprintf("page_alloc: kernel out of memory\n");
        for (;;)
            ;
    }

    page_list = page_list->next;

#ifdef KERNEL_DEBUG
    kprintf("page_alloc: allocating page starting at: %p\n", (uint64_t) node);
#endif

    /* Always zero out free pages on allocation. */
    memset(node, 0x00, PAGE_SIZE);
    return (void *) node;
}

void page_free(void *page_addr)
{
    if ((uint64_t) page_addr >= MEM_TOP || (uint64_t) page_addr < KMEM_END) {
        kprintf("page_free: address outside of physical memory.");
        for (;;)
            ;
    }

    if ((uint64_t) page_addr % PAGE_SIZE != 0) {
        kprintf("page_free: unaligned page address.");
        for (;;)
            ;
    }

#ifdef KERNEL_DEBUG
    memset(page_addr, 0xCC, PAGE_SIZE);
    kprintf("page_free: freeing the page starting at: %p\n", (uint64_t) page_addr);
#endif

    struct free_list_node *node = (struct free_list_node *) page_addr;

    /* Set the first 8 bytes in the page to the address of next free page. */
    node->next = page_list;
    page_list = node;
}
