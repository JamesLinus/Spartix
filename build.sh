#!/bin/sh
set -e
. ./headers.sh

for PROJECT in $PROJECTS; do
  DESTDIR="$PWD/sysroot" $MAKE -C $PROJECT install
done
i686-elf-nm kernel/vmspartix > Kernel.map
i686-elf-nm -C kernel/vmspartix > Kernel-demang.map
i686-elf-strip sysroot/boot/vmspartix
cp Kernel.map sysroot/boot/Kernel.map
cp Kernel-demang.map sysroot/boot/Kernel-demang.map