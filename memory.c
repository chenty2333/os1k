#include "memory.h"
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
      memset((void *)paddr, 0, n * PAGE_SIZE);
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
