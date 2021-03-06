#!/bin/bash

fs=$2

folder=~/nos/$1
loop=`sudo losetup -f`
image=$folder/NOS-$fs.img
mnt=/mnt/floppy

dd if=/dev/zero of=$image bs=512 count=2880
sudo losetup $loop $image

case $fs in
	fat12)
		sudo mkdosfs -F 12 $loop
		;;
	ext2)
		sudo mke2fs $loop
		;;
	*)
		exit 1
		;;
esac

sudo mkdir -p $mnt
sudo mount $loop $mnt
sudo mkdir -p $mnt/nos $mnt/boot

case $fs in
	fat12)
		sudo grub-install --target=i386-pc --boot-directory=$mnt/boot --install-modules="multiboot configfile fat" --fonts=ascii --force --allow-floppy $loop
		;;
	ext2)
		sudo grub-install --target=i386-pc --boot-directory=$mnt/boot --install-modules="multiboot configfile ext2" --fonts=ascii --force --allow-floppy $loop
		;;
	*)
		exit 1
		;;
esac

sudo cp ~/nos/source/.circleci/grub.cfg $mnt/boot/grub/
for conf in mp-acpi mp-noacpi sp-acpi sp-noacpi
do
	sudo mkdir -p $mnt/nos/$conf
	sudo cp $folder/$conf/Kernel.elf $mnt/nos/$conf/
done
sudo umount $loop
sudo sync
sudo losetup -d $loop
bzip2 $image
