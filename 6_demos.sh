#!/bin/sh

. ./config.sh

mkdir -p ${ROOTFS_OVERLAY}/opt

${CROSS_COMPILE}gcc -O2 -g demo_races/compiler_opts.c -o $ROOTFS_OVERLAY/opt/compiler_opts
${CROSS_COMPILE}gcc -O2 -g demo_races/cpu_opts.c -o $ROOTFS_OVERLAY/opt/cpu_opts
${CROSS_COMPILE}gcc -O2 -g demo_races/icache.c -o $ROOTFS_OVERLAY/opt/icache
${CROSS_COMPILE}gcc -O2 -g demo_races/small.c -o $ROOTFS_OVERLAY/opt/small
${CROSS_COMPILE}gcc -O0 -g demo_races/small.c -o $ROOTFS_OVERLAY/opt/small_no_opt
${CROSS_COMPILE}gcc -O2 -g -fno-stack-protector demo_races/memset.c -o $ROOTFS_OVERLAY/opt/memset
${CROSS_COMPILE}gcc -O2 -g -fno-stack-protector demo_races/alloc.c -o $ROOTFS_OVERLAY/opt/alloc

cp --preserve=mode demo_races/gdbs.sh $ROOTFS_OVERLAY/opt
mkdir -p $ROOTFS_OVERLAY/etc/profile.d
cp demo_races/overcommit.sh $ROOTFS_OVERLAY/etc/profile.d
mkdir -p $ROOTFS_OVERLAY/root/.ssh
cp demo_races/authorized_keys $ROOTFS_OVERLAY/root/.ssh
mkdir -p $ROOTFS_OVERLAY/etc/dropbear
cp demo_races/dropbear_ed25519_host_key $ROOTFS_OVERLAY/etc/dropbear

CC=${CROSS_COMPILE}gcc make -C demo_races/pagemap
cp demo_races/pagemap/pagemap $ROOTFS_OVERLAY/opt/

gcc -O2 -g -pthread demo_races/icache.c -o output/icache
gcc -O2 -g -pthread demo_races/cpu_opts.c -o output/cpu_opts

#make ARCH=riscv CROSS_COMPILE=$CROSS_COMPILE -C linux M=$PWD/demo_races
#cp demo_races/demo_races.ko $ROOTFS_OVERLAY/opt
