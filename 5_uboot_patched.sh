#!/bin/sh

. ./config.sh

cd u-boot

# swig 4.5 patch
sed -i \
  -e 's/PyInt_AsLong/PyLong_AsLong/g' \
  -e 's/PyString_FromString/PyUnicode_FromString/g' \
  -e 's/PyString_AsString/PyBytes_AsString/g' \
  scripts/dtc/pylibfdt/libfdt.i_shipped

git clean -fdx
#make CROSS_COMPILE=$CROSS_COMPILE qemu-riscv64_spl_defconfig
cp ../u-boot_config .config
make CROSS_COMPILE=$CROSS_COMPILE OPENSBI=$SBI_OUT LOCALVERSION= -j$JOBS -s
cp spl/u-boot-spl $UBOOT_SPL
cp u-boot.itb $UBOOT_PROPER
#cp build/platform/generic/firmware/fw_dynamic.elf $SBI_ELF
