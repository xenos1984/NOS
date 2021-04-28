#!/bin/bash

fs=$2
#prep="http://hexagon.fi.tartu.ee/~manuel/$fs.img.bz2"

folder=~/nos/$1
loop=`losetup -f`
image=$folder/NOS-$fs.img
mnt=/mnt/floppy

#wget -O $image.bz2 $prep
#bunzip2 $image.bz2

dd if=/dev/zero of=$image bs=512 count=2880
losetup $loop $image

case $fs in
	fat12)
		mkdosfs -F 12 $loop
		;;
	ext2)
		mke2fs $loop
		;;
	*)
		exit 1
		;;
esac

mkdir -p $mnt
mount $loop $mnt
mkdir -p $mnt/nos $mnt/boot

case $fs in
	fat12)
		grub-install --target=i386-pc --boot-directory=$mnt/boot --install-modules="multiboot configfile fat" --fonts=ascii --force --allow-floppy $loop
		;;
	ext2)
		grub-install --target=i386-pc --boot-directory=$mnt/boot --install-modules="multiboot configfile ext2" --fonts=ascii --force --allow-floppy $loop
		;;
	*)
		exit 1
		;;
esac

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
