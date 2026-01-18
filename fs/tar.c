#include "common.h"
#include "device/disk.h"
#include "fs.h"
#include "kernel.h"

// Define struct tar_header locally as it's specific to the TAR format
struct tar_header {
  char name[100];
  char mode[8];
  char uid[8];
  char gid[8];
  char size[12];
  char mtime[12];
  char checksum[8];
  char type;
  char linkname[100];
  char magic[6];
  char version[2];
  char uname[32];
  char gname[32];
  char devmajor[8];
  char devminor[8];
  char prefix[155];
  char padding[12];
  char data[];
} __attribute__((packed));

uint8_t disk[DISK_MAX_SIZE];

int oct2int(char *oct, int len) {
  int dec = 0;
  for (int i = 0; i < len; i++) {
    if (oct[i] < '0' || oct[i] > '7')
      break;

    dec = dec * 8 + (oct[i] - '0');
  }
  return dec;
}

void fs_init(void) {
  for (unsigned sector = 0; sector < sizeof(disk) / SECTOR_SIZE; sector++)
    read_write_disk(&disk[sector * SECTOR_SIZE], sector, false);

  unsigned off = 0;
  for (int i = 0; i < FILES_MAX; i++) {
    struct tar_header *header = (struct tar_header *)&disk[off];
    if (header->name[0] == '\0')
      break;

    if (strcmp(header->magic, "ustar") != 0)
      PANIC("invalid tar header: magic=\"%s\"", header->magic);

    int filesz = oct2int(header->size, sizeof(header->size));
    struct file *file = &files[i];
    file->in_use = true;
    strcpy(file->name, header->name);
    memcpy(file->data, header->data, filesz);
    file->size = filesz;
    printf("file: %s, size=%d\n", file->name, file->size);

    off += align_up(sizeof(struct tar_header) + filesz, SECTOR_SIZE);
  }
}

void fs_flush(void) {
  memset(disk, 0, sizeof(disk));
  unsigned off = 0;
  for (int file_i = 0; file_i < FILES_MAX; file_i++) {
    struct file *file = &files[file_i];
    if (!file->in_use)
      continue;

    struct tar_header *header = (struct tar_header *)&disk[off];
    memset(header, 0, sizeof(*header));
    strcpy(header->name, file->name);
    strcpy(header->mode, "000644");
    strcpy(header->magic, "ustar");
    strcpy(header->version, "00");
    header->type = '0';

    int filesz = file->size;
    for (int i = sizeof(header->size); i > 0; i--) {
      header->size[i - 1] = (filesz % 8) + '0';
      filesz /= 8;
    }

    int checksum = ' ' * sizeof(header->checksum);
    for (unsigned i = 0; i < sizeof(struct tar_header); i++)
      checksum += (unsigned char)disk[off + i];

    for (int i = 5; i >= 0; i--) {
      header->checksum[i] = (checksum % 8) + '0';
      checksum /= 8;
    }

    memcpy(header->data, file->data, file->size);
    off += align_up(sizeof(struct tar_header) + file->size, SECTOR_SIZE);
  }

  for (unsigned sector = 0; sector < sizeof(disk) / SECTOR_SIZE; sector++)
    read_write_disk(&disk[sector * SECTOR_SIZE], sector, true);

  printf("wrote %d bytes to disk\n", sizeof(disk));
}
