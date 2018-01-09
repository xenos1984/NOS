#!/bin/bash

folder=~/nos/$1
loop=`losetup -f`
image=$folder/NOS.img
mnt=/mnt/floppy

dd if=/dev/zero of=$image bs=512 count=2880
losetup $loop $image
mke2fs $loop
mkdir -p $mnt
mount $loop $mnt
mkdir -p $mnt/boot $mnt/nos
grub-install --target=i386-pc --boot-directory=$mnt/boot --install-modules="multiboot configfile ext2" --locales=en_GB --fonts=ascii --force --allow-floppy $loop
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

