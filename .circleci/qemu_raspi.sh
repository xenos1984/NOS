#!/bin/bash

dir=$1

shift 1

sh -c 'sleep 30; echo "quit"' | qemu-system-arm -m 256 -M raspi2 -cpu cortex-a7 -display none -monitor stdio -serial file:$dir/qemu.txt -kernel $dir/Kernel.elf -d cpu_reset -D qemu.log $@

