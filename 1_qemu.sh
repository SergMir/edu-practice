#!/bin/sh

. ./config.sh

mkdir qemu_build
cd qemu_build
../qemu/configure --target-list=riscv64-softmmu --disable-rdma
make -j$JOBS -s
