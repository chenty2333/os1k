#include "memory.h"
#include "common.h"
#include "kernel.h"

static uint8_t page_bitmap[MAX_PAGES / 8];
extern char __free_ram[], __free_ram_end[];

#define BITMAP_SET(i) (page_bitmap[(i) / 8] |= (1 << ((i) % 8)))
#define BITMAP_CLEAR(i) (page_bitmap[(i) / 8] &= ~(1 << ((i) % 8)))
#define BITMAP_TEST(i) (page_bitmap[(i) / 8] & (1 << ((i) % 8)))

paddr_t alloc_pages(uint32_t n) {
  for (uint32_t i = 0; i <= MAX_PAGES - n; i++) {
    bool found = true;
    for (uint32_t j = 0; j < n; j++) {
      if (BITMAP_TEST(i + j)) {
        found = false;
        break;
      }
    }
    if (found) {
      for (uint32_t j = 0; j < n; j++)
        BITMAP_SET(i + j);
      paddr_t paddr = (paddr_t)__free_ram + i * PAGE_SIZE;
      memset((void *)paddr, 0, n * PAGE_SIZE); // Clean memory is happy memory
      return paddr;
    }
  }
  PANIC("out of memory");
}

void free_pages(paddr_t paddr, uint32_t n) {
  uint32_t page_index = (paddr - (paddr_t)__free_ram) / PAGE_SIZE;
  for (uint32_t i = 0; i < n; i++) {
    BITMAP_CLEAR(page_index + i);
  }
}

void map_page(uint32_t *table1, uint32_t vaddr, paddr_t paddr, uint32_t flags) {
  if (!is_aligned(vaddr, PAGE_SIZE))
    PANIC("unaligned vaddr %x", vaddr);

  if (!is_aligned(paddr, PAGE_SIZE))
    PANIC("unaligned paddr %x", paddr);

  /* 1. Extract the L1 index (VPN1) */
  uint32_t vpn1 = VPN1(vaddr);

  /* 2. Check if the L2 page table exists */
  if ((table1[vpn1] & PAGE_V) == 0) {
    /* Create a new L2 page table */
    uint32_t pt_paddr = alloc_pages(1);
    table1[vpn1] = ((pt_paddr / PAGE_SIZE) << 10) | PAGE_V;
  }

  /* 3. Extract the L0 index (VPN0) */
  uint32_t vpn0 = VPN0(vaddr);

  /* 4. Get the pointer to the L2 page table */
  /* PTE_PPN extracts the physical page number of the L2 table, multiply by PAGE_SIZE to get the address */
  uint32_t *table0 = (uint32_t *)(PTE_PPN(table1[vpn1]) * PAGE_SIZE);

  /* 5. Set the Entry in L2 table to map the physical page */
  /* PPN | Flags | Valid */
  table0[vpn0] = ((paddr / PAGE_SIZE) << 10) | flags | PAGE_V;
}
