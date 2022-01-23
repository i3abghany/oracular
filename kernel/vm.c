#include <kernel/kmem.h>
#include <kernel/vm.h>

static pagetable_t kpagetable;

static pagetable_t kpt_create()
{
    pagetable_t kernel_table = (pagetable_t) page_alloc();
    map_npages(kernel_table, UART0_BASE, UART0_BASE, 1, PTE_W_MASK | PTE_R_MASK);
    return kernel_table;
}

static bool entry_is_valid(pte_t entry) { return entry & PTE_V_MASK; }

/*
 * The permission bits, R, W, and X, indicate whether the page is readable, writable, and
 * executable, respectively.  When all three are zero, the PTE is a pointer to the next
 * level of the page table; otherwise, it is a leaf PTE.
 */
static bool entry_is_leaf(pte_t entry)
{
    return entry & (PTE_R_MASK | PTE_X_MASK | PTE_W_MASK);
}

void kvm_init() { kpagetable = kpt_create(); }

void kmap(uint64_t vaddr, uint64_t phys_addr, uint64_t perm)
{
    map(kpagetable, vaddr, phys_addr, perm);
}

void map_npages(pagetable_t table, uint64_t vstart, uint64_t pstart, uint64_t n_pages,
                uint64_t perm)
{
    uint64_t vcurr = vstart;
    uint64_t pcurr = pstart;

    for (uint64_t i = 0; i < n_pages; i++) {
        map(table, vcurr, pcurr, perm);
        vcurr += PAGE_SIZE;
        pcurr += PAGE_SIZE;
    }
}

void map(pagetable_t table, uint64_t vaddr, uint64_t phys_addr, uint64_t perm)
{
    if (vaddr & PAGE_MASK) {
        panic("unaligned virtual address");
    }

    if (phys_addr & PAGE_MASK) {
        panic("unaligned physical address");
    }

    uint64_t vpns[3] = {
        (vaddr >> 12) & 0x1FF,
        (vaddr >> 21) & 0x1FF,
        (vaddr >> 30) & 0x1FF,
    };

    pte_t *pte;
    for (int level = 2; level > 0; level--) {
        pte = &table[vpns[level]];
        if (!entry_is_valid(*pte)) {
            void *pa = page_alloc();
            table = (pagetable_t) pa;
            *pte = ((((uint64_t) pa) >> 12) << 10);
            *pte |= PTE_V_MASK;
        } else {
            uint64_t pa = ((((uint64_t) pte) >> 10) << 12);
            table = (pagetable_t) pa;
        }
    }

    pte = &table[vpns[0]];
    if (entry_is_valid(*pte)) {
        panic("remapping a page");
    }

    *pte = ((phys_addr >> 12) << 10);
    *pte |= perm;
    *pte |= PTE_V_MASK;

    if (!entry_is_leaf(*pte)) {
        panic("last-level pte can't be permessionless");
    }
}
