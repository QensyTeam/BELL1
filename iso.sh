#!/bin/sh
set -e

cd kernel/kernel
objcopy -I binary -O elf32-i386 -B i386 font.psf font.o
cd ../../

. ./build.sh

mkdir -p isodir
mkdir -p isodir/boot
mkdir -p isodir/boot/grub

cp sysroot/boot/BELL.kernel isodir/boot/BELL.kernel
cat > isodir/boot/grub/grub.cfg << EOF

insmod vbe
insmod vga
set gfxpayload=keep

set menu_color_normal=black/cyan
set menu_color_highlight=black/light-cyan

set timeout=5
set default=0


menuentry "BELL" {
    multiboot /boot/BELL.kernel
}
EOF


grub-mkrescue -o BELL.iso isodir
