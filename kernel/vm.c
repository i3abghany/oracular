#include <kernel/kassert.h>
#include <kernel/kmem.h>
#include <kernel/plic.h>
#include <kernel/rv.h>
#include <kernel/virtio.h>
#include <kernel/vm.h>

static pagetable_t kpagetable;

extern char _text_start[];
extern char _text_end[];

extern char _data_start[];
extern char _physical_mem_end[];

static void map_text_section(pagetable_t kernel_table)
{
    uint64_t tstart = (uint64_t) _text_start;
    uint64_t tend = (uint64_t) _text_end;
    map_range(kernel_table, tstart, tstart, tend - tstart, PTE_X_MASK | PTE_R_MASK);
}

/*
 * We map all "data" sections starting from the .data to the end of the memory image.
 * Hence, data section must be defined directly after the .text section.
 */
static void map_data_section(pagetable_t kernel_table)
{
    uint64_t dstart = (uint64_t) _data_start;
    uint64_t dend = (uint64_t) _physical_mem_end;
    map_range(kernel_table, dstart, dstart, dend - dstart, PTE_R_MASK | PTE_W_MASK);
}

static void map_plic_pages(pagetable_t kernel_table)
{
    uint64_t plic_start = (uint64_t) PLIC_BASE;
    uint64_t plic_end = (uint64_t) PLIC_CLAIM;
    uint64_t n_pages = (plic_end - plic_start + PAGE_SIZE - 1) / PAGE_SIZE;
    map_npages(kernel_table, PLIC_BASE, PLIC_BASE, n_pages, PTE_W_MASK | PTE_R_MASK);
}

static void map_virtio(pagetable_t kernel_table)
{
    map_npages(kernel_table, VIRTIO_BASE, VIRTIO_BASE, 1, PTE_W_MASK | PTE_R_MASK);
}

static pagetable_t create_kernel_pagetable()
{
    pagetable_t kernel_table = (pagetable_t) page_alloc();
    map_npages(kernel_table, UART0_BASE, UART0_BASE, 1, PTE_W_MASK | PTE_R_MASK);
    map_plic_pages(kernel_table);
    map_virtio(kernel_table);
    map_text_section(kernel_table);
    map_data_section(kernel_table);
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

void kvm_init()
{
    kpagetable = create_kernel_pagetable();
    uint64_t table_addr = (uint64_t) kpagetable;
    uint64_t ppn = table_addr >> 12;
    uint64_t satp_value = (SATP_MODE_SV39 << 60) | ppn;
    set_satp(satp_value);
}

pagetable_t get_kernel_pagetable() { return kpagetable; }

void kmap(uint64_t vaddr, uint64_t phys_addr, uint64_t perm)
{
    kassert(kpagetable);
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

void map_range(pagetable_t table, uint64_t vstart, uint64_t pstart, uint64_t size,
               uint64_t perm)
{
#ifdef KERNEL_DEBUG
    kprintf("map_range: vs: %p, ps: %p\n", vstart, pstart);
    kprintf("map_range: size: %d\n", size);
#endif
    kassert((vstart & PAGE_MASK) == 0);
    kassert((pstart & PAGE_MASK) == 0);

    // TODO: relieve this constraint (by rounding up to the nearest n pages).
    kassert(((vstart + size) & PAGE_MASK) == 0);
    kassert(((pstart + size) & PAGE_MASK) == 0);

    uint64_t n_pages = size / PAGE_SIZE;
    map_npages(table, vstart, pstart, n_pages, perm);
}

// TODO: Support coarser-grained pages by stopping at a certain level of
// translation
void map(pagetable_t table, uint64_t vaddr, uint64_t phys_addr, uint64_t perm)
{
    if (vaddr & PAGE_MASK) {
        panic("map: unaligned virtual address: 0x%p", vaddr);
    }

    if (phys_addr & PAGE_MASK) {
        panic("map: unaligned physical address: 0x%p", phys_addr);
    }

#ifdef KERNEL_DEBUG
    kprintf("map: vaddr: 0x%p, paddr: 0x%p\n", vaddr, phys_addr);
#endif

    uint64_t vpns[3] = {
        (vaddr >> 12) & 0x1FF,
        (vaddr >> 21) & 0x1FF,
        (vaddr >> 30) & 0x1FF,
    };

    pte_t *pte;
    for (int level = 2; level > 0; level--) {
        pte = &table[vpns[level]];
        if (entry_is_valid(*pte)) {
            uint64_t pa = ((((uint64_t) *pte) >> 10) << 12);
            table = (pagetable_t) pa;
        } else {
            void *pa = page_alloc();
            table = (pagetable_t) pa;
            *pte = ((((uint64_t) pa) >> 12) << 10);
            *pte |= PTE_V_MASK;
        }
    }

    pte = &table[vpns[0]];
    if (entry_is_valid(*pte)) {
        panic("map: remapping a page");
    }

    *pte = (((phys_addr >> 12) & 0x1FF) << 10);
    *pte |= (((phys_addr >> 21) & 0x1FF) << 19);
    *pte |= (((phys_addr >> 30) & 0x3FFFFFF) << 28);
    *pte |= perm;
    *pte |= PTE_V_MASK;

    if (!entry_is_leaf(*pte)) {
        panic("map: last-level pte can't be permission-less");
    }
}

/*
 * FIXME: Should not be called with an L1 or L0 table for now.
 */
void unmap_table(pagetable_t table)
{
    for (int i = 0; i < ENTRIES_PER_PAGE_TABLE; i++) {
        pte_t l2_entry = table[i];
        // TODO: Support leafs in level 1. Currently, we assume that leafs are
        // only in level 0.
        if (entry_is_valid(l2_entry)) {
            pagetable_t l1_table = (pagetable_t) ((l2_entry & ~(0x3FF)) << 2);
            for (int j = 0; j < ENTRIES_PER_PAGE_TABLE; j++) {
                pte_t l1_entry = l1_table[j];
                if (entry_is_valid(l1_entry)) {
                    pagetable_t l0_table = (pagetable_t) ((l1_entry & ~(0x3FF)) << 2);
                    page_free((void *) l0_table);
                }
            }
            page_free((void *) l1_table);
        }
    }
}

uint64_t translate_address(pagetable_t table, uint64_t vaddr)
{
    uint64_t vpns[3] = {
        (vaddr >> 12) & 0x1FF,
        (vaddr >> 21) & 0x1FF,
        (vaddr >> 30) & 0x1FF,
    };

    pte_t *pte;
    for (int level = 2; level > 0; level--) {
        pte = &table[vpns[level]];
        if (!entry_is_valid(*pte)) {
            return 0;
        }

        // FIXME: leaves can be at any level, we currently only consider 4 KiB
        // pages, i.e. 3 levels of nesting.

        uint64_t pa = ((((uint64_t) *pte) >> 10) << 12);
        table = (pagetable_t) pa;
    }

    pte = &table[vpns[0]];
    if (!entry_is_valid(*pte)) {
        return 0;
    }

    return (((uint64_t) *pte >> 10) << 12) | (vaddr & 0xFFFF);
}
