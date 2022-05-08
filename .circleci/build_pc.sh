#!/bin/bash

target=$1
folder=~/nos/$target/$2
shift 2

./configure --host=$target $@
make 2>&1 | tee build.log

mkdir -p $folder
cp config.log $folder/
cp build.log $folder/
cp kernel/vendor/pc/Kernel.elf $folder/
cp kernel/vendor/pc/Kernel.map $folder/
#cp drivers/pc/floppy/Floppy.elf $folder/
cp images/pc/NOS.iso $folder/
$target-objdump -x -d -C -g -S $folder/Kernel.elf > $folder/Kernel.dump

make clean
