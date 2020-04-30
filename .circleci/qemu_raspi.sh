#!/bin/bash

arch=$1
mach=$2
cpu=$3
dir=$4

shift 4

sh -c 'sleep 10; echo "quit"' | qemu-system-$arch -m 1024 -M $mach -cpu $cpu -display none -monitor stdio -serial file:$dir/qemu-$mach.txt -kernel $dir/Kernel.elf -d cpu_reset -D $dir/qemu-$mach.log $@

