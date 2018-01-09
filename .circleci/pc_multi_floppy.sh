#!/bin/bash

fs=$2
prep="http://hexagon.fi.tartu.ee/~manuel/$fs.img.bz2"

folder=~/nos/$1
loop=`losetup -f`
image=$folder/NOS-$fs.img
mnt=/mnt/floppy

wget -O $image.bz2 $prep
bunzip2 $image.bz2

#dd if=/dev/zero of=$image bs=512 count=2880
losetup $loop $image
#mkdosfs -F 12 $loop
mkdir -p $mnt
mount $loop $mnt
mkdir -p $mnt/nos #$mnt/boot
#grub-install --target=i386-pc --boot-directory=$mnt/boot --install-modules="multiboot configfile fat" --fonts=ascii --force --allow-floppy --debug --debug -v $loop
cp ~/nos/source/.circleci/grub.cfg $mnt/boot/grub/
for conf in mp-acpi mp-noacpi sp-acpi sp-noacpi
do
	mkdir -p $mnt/nos/$conf
	cp $folder/$conf/Kernel.elf $mnt/nos/$conf/
done
umount $loop
sync
losetup -d $loop
bzip2 $image

