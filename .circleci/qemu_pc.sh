#!/bin/bash

arch=$1
mach=$2
smp=$3
dir=$4

shift 4

sh -c 'sleep 10; echo "quit"' | qemu-system-$arch -m 256 -M $mach -smp $smp -display none -monitor stdio -debugcon file:$dir/qemu-$mach-$smp.txt -drive file=$dir/NOS.iso,media=cdrom,id=d -boot order=d -d cpu_reset -D $dir/qemu-$mach-$smp.log $@

