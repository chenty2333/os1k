#!/bin/bash
set -xue

# QEMU file path
QEMU=qemu-system-riscv32

# Clang's path and compile flags
CC=clang
CFLAGS="-std=c11 -O2 -g3 -Wall -Wextra --target=riscv32-unknown-elf -fuse-ld=lld -fno-stack-protector -ffreestanding -nostdlib -Iinclude -I."

OBJCOPY=llvm-objcopy

$CC $CFLAGS -Wl,-Tuser/user.ld -Wl,-Map=shell.map -o shell.elf user/shell.c user/crt.c user/user_lib.c common/common.c
$OBJCOPY --set-section-flags .bss=alloc,contents -O binary shell.elf shell.bin
$OBJCOPY -Ibinary -Oelf32-littleriscv shell.bin shell.bin.o

$CC $CFLAGS -Wl,-Tkernel.ld -Wl,-Map=kernel.map -o kernel.elf \
    kernel/main.c kernel/trap.c kernel/process.c kernel/syscall.c kernel/sbi.c kernel/panic.c \
    mm/memory.c device/disk.c fs/fs.c fs/tar.c common/common.c shell.bin.o

(cd disk && tar cf ../disk.tar --format=ustar *.txt)

# start QEMU
$QEMU -machine virt -bios default -nographic -serial mon:stdio --no-reboot \
    -d unimp,guest_errors,int,cpu_reset -D qemu.log \
    -drive id=drive0,file=disk.tar,format=raw,if=none \
    -device virtio-blk-device,drive=drive0,bus=virtio-mmio-bus.0 \
    -kernel kernel.elf
