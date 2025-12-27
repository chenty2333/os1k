#include "kernel.h"
#include "common.h"
#include "memory.h"
#include "process.h"
#include "riscv.h"
#include "sbi.h"
#include "trap.h"

extern char __bss[], __bss_end[], __stack_top[];
extern void kernel_entry(void); // Defined in trap.c

void delay(void) {
  for (int i = 0; i < 30000000; i++)
    __asm__ __volatile__("nop");
}

struct process *proc_a;
struct process *proc_b;

void proc_a_entry(void) {
  printf("Starting process A\n");
  while (1) {
    putchar('A');
    switch_context(&proc_a->sp, &proc_b->sp);
    delay();
  }
}

void proc_b_entry(void) {
  printf("Starting process B\n");
  while (1) {
    putchar('B');
    switch_context(&proc_b->sp, &proc_a->sp);
    delay();
  }
}

void kernel_main(void) {
  memset(__bss, 0, (size_t)__bss_end - (size_t)__bss);

  printf("\n\nHello %s\n", "World!");
  printf("1 + 2 = %d, %x\n", 1 + 2, 0x1234abcd);
  paddr_t paddr0 = alloc_pages(2);
  paddr_t paddr1 = alloc_pages(1);
  printf("alloc_pages test: paddr0=%x\n", paddr0);
  printf("alloc_pages test: paddr1=%x\n", paddr1);

  idle_proc = create_process((uint32_t)NULL);
  idle_proc->pid = 0;
  current_proc = idle_proc;

  proc_a = create_process((uint32_t)proc_a_entry);
  proc_b = create_process((uint32_t)proc_b_entry);

  yield();

  for (;;) {
    __asm__ __volatile__("wfi");
  }
}

__attribute__((section(".text.boot"))) __attribute__((naked)) void boot(void) {
  __asm__ __volatile__("mv sp, %[stack_top]\n"
                       "j kernel_main\n"
                       :
                       : [stack_top] "r"(__stack_top));
}
