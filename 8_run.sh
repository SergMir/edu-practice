#/bin/sh

    # -cpu rv64,v=true,vlen=128,sscofpmf=true \
    # -cpu rv64,v=false,sscofpmf=true \
    # -cpu rv64,v=false,h=false,sscofpmf=true \
    # варианты флагов чтобы вывод немного отличался

. ./config.sh

./qemu_build/qemu-system-riscv64 \
    -smp 4 \
    -machine virt \
    -cpu rv64,v=false,h=false,sscofpmf=true \
    -m 256m \
    -nographic \
    -bios $SBI_OUT \
    -kernel $KERNEL_OUT \
    -initrd $ROOTS_OUT \
    -append "console=ttyS0 earlycon root=/dev/ram0 rw init=/init" \
    -device virtio-net-device,netdev=net \
    -netdev user,id=net,hostfwd=tcp::2345-:2345,hostfwd=tcp::10022-:22 \
    -gdb tcp::1234 \
    -monitor telnet:127.0.0.1:1122,server,nowait \
    -serial mon:stdio \
    $1
