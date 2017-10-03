#!/bin/bash

sh -c 'sleep 45; echo "quit"' | qemu-system-arm -m 256 -M raspi2 -cpu cortex-a7 -display none -monitor stdio -serial file:$1/qemu.txt -kernel $1/Kernel.elf

