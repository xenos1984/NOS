#!/bin/bash

target=$1
folder=~/$target/$2
shift 2

./configure --host=$target $@
make 2>&1 | tee build.log

mkdir -p $folder
cp config.log $folder/
cp build.log $folder/
cp kernel/arch/x86/Kernel.elf $folder/
cp drivers/pc/floppy/Floppy.elf $folder/
cp images/x86/NOS.iso $folder/

make clean

