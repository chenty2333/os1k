#include "kernel.h"
#include "common.h"
#include "memory.h"
#include "process.h"
#include "riscv.h"
#include "sbi.h"
#include "syscall.h"
#include "trap.h"

extern char __bss[], __bss_end[], __stack_top[];
extern void kernel_entry(void); // Defined in trap.c
extern char _binary_shell_bin_start[], _binary_shell_bin_size[];

long getchar(void) {
  struct sbiret ret = sbi_call(0, 0, 0, 0, 0, 0, 0, 2);
  return ret.error;
}

void handle_syscall(struct trap_frame *f) {
  switch (f->a3) {
  case SYS_PUTCHAR:
    putchar(f->a0);
    break;
  case SYS_GETCHAR:
    while (1) {
      long ch = getchar();
      if (ch >= 0) {
        f->a0 = ch;
        break;
      }
      yield();
    }
    break;
  case SYS_EXIT:
    printf("process %d exited\n", current_proc->pid);
    current_proc->state = PROC_EXITED;
    yield();
    PANIC("unreachable");
  default:
    PANIC("unexpected syscall a3=%x\n", f->a3);
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

  WRITE_CSR(stvec, (uint32_t)kernel_entry);

  idle_proc = create_process(NULL, 0);
  idle_proc->pid = 0;
  current_proc = idle_proc;

  create_process(_binary_shell_bin_start, (size_t)_binary_shell_bin_size);

  yield();
  PANIC("switched to idle process");
}

__attribute__((section(".text.boot"))) __attribute__((naked)) void boot(void) {
  __asm__ __volatile__("mv sp, %[stack_top]\n"
                       "j kernel_main\n"
                       :
                       : [stack_top] "r"(__stack_top));
}
