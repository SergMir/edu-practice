#!/bin/sh

. ./config.sh

cp buildroot_config buildroot/.config
cd buildroot
make sdk
cd ../output
rm -rf toolchain
tar xf ../buildroot/output/images/riscv64-buildroot-linux-gnu_sdk-buildroot.tar.gz
mv riscv64-buildroot-linux-gnu_sdk-buildroot toolchain
