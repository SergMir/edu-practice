#!/bin/sh

. ./config.sh

cd u-boot
git clean -fdx
#make CROSS_COMPILE=$CROSS_COMPILE qemu-riscv64_spl_defconfig
cp ../u-boot_config .config
make CROSS_COMPILE=$CROSS_COMPILE OPENSBI=$SBI_OUT -j$JOBS -s
cp spl/u-boot-spl $UBOOT_SPL
cp u-boot.itb $UBOOT_PROPER
#cp build/platform/generic/firmware/fw_dynamic.elf $SBI_ELF
