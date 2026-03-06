#/bin/sh

. ./config.sh

./output/toolchain/bin/riscv64-linux-gdb -ix ./buildroot/output/staging/usr/share/buildroot/gdbinit --command ./app.gdb --se=$1
