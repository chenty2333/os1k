#include "syscall.h"
#include "common.h"
#include "device/disk.h"
#include "fs/fs.h"
#include "kernel.h"
#include "process.h"
#include "sbi.h"
#include "trap.h"

extern struct process *current_proc;
extern void yield(void);

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
  case SYS_READFILE:
  case SYS_WRITEFILE: {
    const char *filename = (const char *)f->a0;
    char *buf = (char *)f->a1;
    int len = f->a2;
    struct file *file = fs_lookup(filename);
    if (!file) {
      printf("file not found: %s\n", filename);
      f->a0 = -1;
      break;
    }

    if (len > (int)sizeof(file->data))
      len = file->size;

    if (f->a3 == SYS_WRITEFILE) {
      memcpy(file->data, buf, len);
      file->size = len;
      fs_flush();
    } else {
      memcpy(buf, file->data, len);
    }

    f->a0 = len;
    break;
  }
  default:
    PANIC("unexpected syscall a3=%x\n", f->a3);
  }
}
