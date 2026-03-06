#!/bin/sh

. ./config.sh

cp linux_config linux/.config
cd linux && make ARCH=riscv CROSS_COMPILE=$CROSS_COMPILE modules_prepare
