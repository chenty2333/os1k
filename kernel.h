#pragma once
#include "common.h"

#define PANIC(fmt, ...)                                                        \
  do {                                                                         \
    printf("PANIC: %s:%d: " fmt "\n", __FILE__, __LINE__, ##__VA_ARGS__);      \
    while (1) {                                                                \
    }                                                                          \
  } while (0)

/*
 * RISC-V Sv32 Page Table related macros.
 * See RISC-V Privileged Architecture Supervisor-Level ISA.
 */
#define SATP_SV32 (1u << 31) /* Enable Sv32 paging mode (32-bit virtual addressing) */

/* Page Table Entry (PTE) flags */
#define PAGE_V (1 << 0) /* Valid bit: Entry is valid */
#define PAGE_R (1 << 1) /* Read bit: Page is readable */
#define PAGE_W (1 << 2) /* Write bit: Page is writable */
#define PAGE_X (1 << 3) /* Execute bit: Page is executable */
#define PAGE_U (1 << 4) /* User bit: Page is accessible in User Mode (U-mode) */

/*
 * PTE field manipulation macros.
 * - PPN: Physical Page Number (Bits 31-10)
 * - Flags: Permission and state bits (Bits 9-0)
 */
#define PTE_PPN(pte) (((pte) >> 10))
#define PTE_FLAGS(pte) ((pte) & 0x3ff)

/*
 * Virtual Address structure in Sv32:
 * [31:22] VPN[1] (L1 Index)
 * [21:12] VPN[0] (L0 Index)
 * [11: 0] Page Offset
 */
#define VPN1(vaddr) (((vaddr) >> 22) & 0x3ff)
#define VPN0(vaddr) (((vaddr) >> 12) & 0x3ff)
#define USER_BASE 0x1000000

#define SSTATUS_SPIE (1 << 5)
#define SSTATUS_SPP (1 << 8)

#define SCAUSE_ECALL 8
#define PROC_EXITED 2