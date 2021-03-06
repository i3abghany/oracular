#ifndef _KERNEL_VM_H
#define _KERNEL_VM_H

#include <kernel/console.h>
#include <stdbool.h>
#include <stdint.h>

/*
 * Sv39 implementations support a 39-bit virtual address space, divided into 4
 * KiB pages. The virtual address is divided into three page numbers, each of
 * which is used to index a pagetable. Each page address subdivision is 9-bit
 * wide, which means that one page table contains 512 PTEs.
 */
#define ENTRIES_PER_PAGE_TABLE 512

#define PTE_V_MASK (1 << 0) /* valid */
#define PTE_R_MASK (1 << 1) /* readable */
#define PTE_W_MASK (1 << 2) /* writable */
#define PTE_X_MASK (1 << 3) /* executable */
#define PTE_U_MASK (1 << 4) /* user */
#define PTE_G_MASK (1 << 5) /* global */
#define PTE_A_MASK (1 << 6) /* accessed */
#define PTE_D_MASK (1 << 7) /* dirty */

#define SATP_MODE_BARE 0ULL
#define SATP_MODE_SV39 8ULL
#define SATP_MODE_SV48 9ULL
#define SATP_MODE_SV57 10ULL

typedef uint64_t pte_t;
typedef uint64_t *pagetable_t;

/*
 * Using a root page table, map the page starting at @vaddr to the physical
 * frame starting at @phys_addr, and gives such page permissions designated by
 * @perm.
 */
void map(pagetable_t table, uint64_t vaddr, uint64_t phys_addr, uint64_t perm);

pagetable_t get_kernel_pagetable();

/*
 * Utility helper to map n consecutive virtual pages to their corresponding n physical
 * frames.
 */
void map_npages(pagetable_t table, uint64_t vstart, uint64_t pstart, uint64_t n_pages,
                uint64_t perm);

/*
 * Utility helper to map the range from @vstart up to @vstart + @size to the corresponding
 * physical frames. Currently, @vstart and @pstart have to be page-aligned. Also, @size
 * must be a multiple of the page size.
 */
void map_range(pagetable_t table, uint64_t vstart, uint64_t pstart, uint64_t size,
               uint64_t perm);

/*
 * Map a kernel page to a physical frame.
 */
void kmap(uint64_t vaddr, uint64_t phys_addr, uint64_t perm);

/*
 * Initialize kernel page table using Sv39 mappings.
 */
void kvm_init();

/*
 * Unmaps and frees all the mapped pages in a three-level nested paging
 * structure.
 */
void unmap_table(pagetable_t table);

/*
 * Translate a virtual address to its physical counterpart using Sv39 mappings.
 */
uint64_t translate_address(pagetable_t table, uint64_t vaddr);

#endif
