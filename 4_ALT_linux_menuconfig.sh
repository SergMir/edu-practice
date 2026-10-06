#!/bin/sh

. ./config.sh

cd linux && make ARCH=riscv CROSS_COMPILE=$CROSS_COMPILE menuconfig; cd ..
#cd linux && make ARCH=riscv CROSS_COMPILE=$CROSS_COMPILE oldconfig; cd ..
cp linux/.config linux_config
