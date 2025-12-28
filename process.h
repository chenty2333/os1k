#pragma once
#include "common.h"

#define PROCS_MAX 8
#define PROC_UNUSED 0
#define PROC_RUNNABLE 1

struct process {
  int pid;
  int state;
  vaddr_t sp;           /* Kernel Stack Pointer */
  uint32_t *page_table; /* Pointer to the first level page table (L1) */
  uint8_t stack[8192];  /* Kernel Stack */
};

/**
 * create_process - Create a new process.
 * @pc: The entry point (program counter) of the process.
 *
 * Allocates a process control block, sets up the initial stack frame,
 * creates a new page table, and identity-maps the kernel memory.
 */
struct process *create_process(uint32_t pc);

/**
 * switch_context - Switch CPU context (callee-saved registers).
 * @prev_sp: Pointer to save the current stack pointer.
 * @next_sp: Pointer to the next stack pointer to load.
 */
__attribute__((naked)) void switch_context(uint32_t *prev_sp,
                                           uint32_t *next_sp);

extern struct process *current_proc;
extern struct process *idle_proc;

/**
 * yield - Voluntarily yield the CPU to the next runnable process.
 *
 * This function handles process scheduling and page table switching.
 */
void yield(void);