#!/bin/bash

arch=$1
mach=$2
cpu=$3
ram=$4
dir=$5

shift 4

sh -c 'sleep 10; echo "quit"' | qemu-system-$arch -m $ram -M $mach -cpu $cpu -display none -monitor stdio -serial file:$dir/qemu-$mach.txt -kernel $dir/Kernel.elf -d cpu_reset -D $dir/qemu-$mach.log $@

