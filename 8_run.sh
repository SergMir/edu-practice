#/bin/sh

. ./config.sh

./qemu_build/qemu-system-riscv64 \
    -smp 4 \
    -machine virt \
    -cpu rv64,v=true,vlen=128,sscofpmf=true \
    -m 256m \
    -nographic \
    -bios $UBOOT_SPL \
    -kernel $KERNEL_OUT \
    -device loader,file=$UBOOT_PROPER,addr=0x80200000 \
    -device loader,file=$KERNEL_OUT,addr=0x88000000 \
    -device loader,file=$ROOTS_OUT,addr=0x8c300000 \
    -append "console=ttyS0 earlycon root=/dev/ram0 rw init=/init" \
    -device virtio-net-device,netdev=net \
    -netdev user,id=net,hostfwd=tcp::2345-:2345,hostfwd=tcp::10022-:22 \
    -gdb tcp::1234 \
    -monitor telnet:127.0.0.1:1122,server,nowait \
    -serial mon:stdio \
    $1
