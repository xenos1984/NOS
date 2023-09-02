#!/bin/bash

here=$(dirname "$0")
bochs=$1
smp=$2
dir=$3

shift 4

$bochs -q -f $here/bochsrc "ata0-master: type=cdrom, path=$dir/NOS.iso, status=inserted" "cpu: count=$smp" "log: $dir/$bochs-$smp.log" > $dir/$bochs-$smp.txt &
pid=$!
sleep 30
pkill $bochs
ansifilter -H -d "Emulator: $bochs Subtype: $dir CPU cores: $smp" --art-tundra -i $dir/$bochs-$smp.txt -o $dir/$bochs-$smp.html
