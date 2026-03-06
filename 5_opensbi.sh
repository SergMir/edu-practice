#!/bin/sh

. ./config.sh

cd opensbi
git clean -fdx
make CROSS_COMPILE=$CROSS_COMPILE PLATFORM=generic FW_OPTIONS=0x2 BUILD_INFO=y V=1 -j$JOBS -s
cp build/platform/generic/firmware/fw_dynamic.bin $SBI_OUT
cp build/platform/generic/firmware/fw_dynamic.elf $SBI_ELF
