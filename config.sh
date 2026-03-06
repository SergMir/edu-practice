#!/bin/sh

# Exit immediately on errors
set -e

# Print what is happening
set -x

JOBS=`nproc`
CROSS_COMPILE=$PWD/output/toolchain/bin/riscv64-buildroot-linux-gnu-

ROOTS_OUT=$PWD/output/rootfs.cpio
KERNEL_OUT=$PWD/output/vmlinux.bin
SBI_OUT=$PWD/output/fw_dynamic.bin
SBI_ELF=$PWD/output/fw_dynamic.elf
ROOTFS_OVERLAY=$PWD/rootfs_overlay
