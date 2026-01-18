#pragma once
#include "common.h"
#include "device/disk.h"

#define FILES_MAX 2
#define DISK_MAX_SIZE align_up(sizeof(struct file) * FILES_MAX, SECTOR_SIZE)

struct file {
  bool in_use;
  char name[100];
  char data[1024];
  size_t size;
};

extern struct file files[FILES_MAX];

void fs_init(void);
void fs_flush(void);
struct file *fs_lookup(const char *filename);
