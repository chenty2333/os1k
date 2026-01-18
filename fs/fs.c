#include "fs.h"
#include "common.h"

struct file files[FILES_MAX];

struct file *fs_lookup(const char *filename) {
  for (int i = 0; i < FILES_MAX; i++) {
    struct file *file = &files[i];
    if (!strcmp(file->name, filename))
      return file;
  }

  return NULL;
}
