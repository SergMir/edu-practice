source ../gef/gef.py
add-symbol-file vmlinux
add-symbol-file ../output/fw_dynamic.elf
source vmlinux-gdb.py
#lx-symbols
gef-remote --qemu-user localhost 1234
