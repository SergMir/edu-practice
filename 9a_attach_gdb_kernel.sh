#/bin/sh

. ./config.sh

cd linux; ../output/toolchain/bin/riscv64-linux-gdb --command ../kernel.gdb vmlinux
