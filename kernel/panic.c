#include "common.h"
#include "kernel.h"

void panic(const char *file, int line, const char *fmt, ...) {
  va_list vargs;
  va_start(vargs, fmt);

  printf("PANIC: %s:%d: ", file, line);
  vprintf(fmt, vargs);
  printf("\n");

  va_end(vargs);

  while (1) {
  }
}
