#!/bin/bash
set -xue

# QEMU file path
QEMU=qemu-system-riscv32

# start QEMU
$QEMU -machine virt -bios default -nographic -serial mon:stdio --no-reboot