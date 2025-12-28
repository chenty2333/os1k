#pragma once
#include "common.h"

/**
 * alloc_pages - Allocate contiguous physical memory pages.
 * @n: Number of pages to allocate.
 *
 * Returns the physical address of the start of the allocated memory.
 * The memory is zero-initialized.
 * PANICs if there is no free memory available.
 */
paddr_t alloc_pages(uint32_t n);

/**
 * free_pages - Free previously allocated physical memory pages.
 * @paddr: Physical address of the start of the memory to free.
 * @n: Number of pages to free.
 *
 * Marks the corresponding bits in the page bitmap as free.
 */
void free_pages(paddr_t paddr, uint32_t n);

/**
 * map_page - Map a virtual page to a physical page in the given page table.
 * @table1: Pointer to the L1 (root) page table.
 * @vaddr:  The virtual address to map (must be 4KB aligned).
 * @paddr:  The physical address to map to (must be 4KB aligned).
 * @flags:  Permission flags (PAGE_R, PAGE_W, PAGE_X, PAGE_U).
 *
 * This function handles the allocation of L2 page tables if necessary.
 */
void map_page(uint32_t *table1, uint32_t vaddr, paddr_t paddr, uint32_t flags);