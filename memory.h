#pragma once
#include "common.h"

paddr_t alloc_pages(uint32_t n);
void free_pages(paddr_t paddr, uint32_t n);
