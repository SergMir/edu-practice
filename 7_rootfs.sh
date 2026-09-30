#!/bin/sh

. ./config.sh

cp buildroot_config buildroot/.config
cd buildroot
make -s
zstd -f ./output/images/rootfs.cpio -o rootfs.cpio.zst
mkimage -A riscv -T ramdisk -C zstd -d rootfs.cpio.zst rootfs.uimage
cp ./rootfs.uimage $ROOTS_OUT
