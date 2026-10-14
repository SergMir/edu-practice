#!/bin/sh

. ./config.sh

mkdir -p ${ROOTFS_OVERLAY}/opt

${CROSS_COMPILE}gcc -O2 -g demos/compiler_opts.c -o $ROOTFS_OVERLAY/opt/compiler_opts
${CROSS_COMPILE}gcc -O2 -g demos/cpu_opts.c -o $ROOTFS_OVERLAY/opt/cpu_opts
${CROSS_COMPILE}gcc -O2 -g demos/icache.c -o $ROOTFS_OVERLAY/opt/icache
${CROSS_COMPILE}gcc -O2 -g demos/small.c -o $ROOTFS_OVERLAY/opt/small
${CROSS_COMPILE}gcc -O0 -g demos/small.c -o $ROOTFS_OVERLAY/opt/small_no_opt
${CROSS_COMPILE}gcc -O2 -g -fno-stack-protector demos/memset.c -o $ROOTFS_OVERLAY/opt/memset
${CROSS_COMPILE}gcc -O2 -g -fno-stack-protector demos/alloc.c -o $ROOTFS_OVERLAY/opt/alloc
${CROSS_COMPILE}gcc -O2 -g demos/qemu_first_task.c -o $ROOTFS_OVERLAY/opt/qemu_first_task

cp --preserve=mode demos/gdbs.sh $ROOTFS_OVERLAY/opt
mkdir -p $ROOTFS_OVERLAY/etc/profile.d
cp demos/overcommit.sh $ROOTFS_OVERLAY/etc/profile.d
mkdir -p $ROOTFS_OVERLAY/root/.ssh
cp demos/authorized_keys $ROOTFS_OVERLAY/root/.ssh
mkdir -p $ROOTFS_OVERLAY/etc/dropbear
cp demos/dropbear_ed25519_host_key $ROOTFS_OVERLAY/etc/dropbear

CC=${CROSS_COMPILE}gcc make -C demos/pagemap
cp demos/pagemap/pagemap $ROOTFS_OVERLAY/opt/

CC=${CROSS_COMPILE}gcc make -C demo_races/pagetree
cp demo_races/pagetree/pagetree $ROOTFS_OVERLAY/opt/

#CC=${CROSS_COMPILE}gcc OBJCOPY=${CROSS_COMPILE}objcopy make -C ../crackme
cp demos/crackme $ROOTFS_OVERLAY/opt/

gcc -O2 -g -pthread demos/icache.c -o output/icache
gcc -O2 -g -pthread demos/cpu_opts.c -o output/cpu_opts

#make ARCH=riscv CROSS_COMPILE=$CROSS_COMPILE -C linux M=$PWD/demos
#cp demos/demos.ko $ROOTFS_OVERLAY/opt
