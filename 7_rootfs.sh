#!/bin/sh

. ./config.sh

cp buildroot_config buildroot/.config
cd buildroot
make -s
cp ./output/images/rootfs.cpio $ROOTS_OUT
