#pragma once

#define SECTOR_SIZE 512

void virtio_blk_init(void);
void read_write_disk(void *buf, unsigned sector, int is_write);
