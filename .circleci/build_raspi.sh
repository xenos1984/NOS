#!/bin/bash

target=$1
folder=~/nos/$target/$2
shift 2

export CFLAGS="$CFLAGS -fdiagnostics-color=always"
./configure --host=$target $@
make 2>&1 | tee build.log
ansifilter -H -d "Build log $target" -i build.log -o build.html
sed -i -r "s/\x1B\[([0-9]{1,2}(;[0-9]{1,2})?)?[m|K]//g" build.log

mkdir -p $folder
cp config.log $folder/
cp build.log $folder/
cp kernel/vendor/raspi/Kernel.elf $folder/
cp kernel/vendor/raspi/Kernel.map $folder/
#cp images/raspi/NOS.img $folder/
$target-objdump -x -s -d -C -g -S --visualize-jumps=extended-color --disassembler-color=extended $folder/Kernel.elf | ansifilter -H -d "Dump $target" > $folder/Kernel.dump.html

make clean
