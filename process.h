#pragma once
#include "common.h"

#define PROCS_MAX 8
#define PROC_UNUSED 0
#define PROC_RUNNABLE 1

struct process {
  int pid;
  int state;
  vaddr_t sp;
  uint8_t stack[8192];
};

struct process *create_process(uint32_t pc);
__attribute__((naked)) void switch_context(uint32_t *prev_sp,
                                           uint32_t *next_sp);

extern struct process *current_proc;
extern struct process *idle_proc;

void yield(void);