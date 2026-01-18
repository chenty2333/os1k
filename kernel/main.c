#include "common.h"
#include "device/disk.h"
#include "fs/fs.h"
#include "kernel.h"
#include "memory.h"
#include "process.h"
#include "riscv.h"
#include "sbi.h"
#include "trap.h"

extern char __bss[], __bss_end[], __stack_top[];
extern void kernel_entry(void); // Defined in trap.c
extern char _binary_shell_bin_start[], _binary_shell_bin_size[];

void kernel_main(void) {
  memset(__bss, 0, (size_t)__bss_end - (size_t)__bss);

  printf("\n\nHello %s\n", "World!");
  printf("1 + 2 = %d, %x\n", 1 + 2, 0x1234abcd);
  paddr_t paddr0 = alloc_pages(2);
  paddr_t paddr1 = alloc_pages(1);
  printf("alloc_pages test: paddr0=%x\n", paddr0);
  printf("alloc_pages test: paddr1=%x\n", paddr1);

  WRITE_CSR(stvec, (uint32_t)kernel_entry);

  virtio_blk_init();
  fs_init();

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
