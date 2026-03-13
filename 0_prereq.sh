#!/bin/sh

. ./config.sh

[ -z "`grep safe-path ~/.gdbinit`" ] && echo 'Add "set auto-load safe-path /" to your ~/.gdbinit (or limit to this directory)'

# Fedora
#sudo dnf install ncurses-devel perl-ExtUtils-MakeMaker ninja-build glib2-devel pixman-devel libslirp-devel perl-IPC-Cmd perl-open lz4 gawk libmpc-devel gmp-devel
# Ubuntu
#sudo apt install libncurses-dev libextutils-makemaker-cpanfile-perl ninja-build libglib2.0-dev libpixman-1-dev libslirp-dev libipc-run-perl lz4 gawk libmpc-dev libgmp-dev

git clone -b 2026.02.x git://git.busybox.net/buildroot
git clone git://git.kernel.org/pub/scm/linux/kernel/git/torvalds/linux.git
cd linux; git checkout v6.19; cd ..
git clone https://github.com/riscv-software-src/opensbi.git
cd opensbi; git checkout v1.8; cd ..
git clone https://github.com/qemu/qemu.git
cd qemu; git checkout v10.2.0; cd ..
git clone https://github.com/hugsy/gef.git
git clone https://github.com/dwks/pagemap.git demo_races/pagemap
sed -i 's/CC = gcc/CC ?= gcc/' demo_races/pagemap/Makefile

mkdir -p output
mkdir -p $ROOTFS_OVERLAY
