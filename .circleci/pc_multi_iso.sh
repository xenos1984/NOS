#!/bin/bash

grub-mkimage -o eltorito.img -O i386-pc-eltorito biosdisk iso9660 multiboot configfile

genisoimage -R -c boot/boot.cat -b boot/grub/eltorito.img -no-emul-boot -boot-load-size 4 -boot-info-table -o $1/NOS.iso -graft-points \
	boot/grub/eltorito.img=eltorito.img \
	boot/grub/grub.cfg=.circleci/grub.cfg \
	nos/mp-acpi/Kernel.elf=$1/mp-acpi/Kernel.elf \
	nos/mp-noacpi/Kernel.elf=$1/mp-noacpi/Kernel.elf \
	nos/sp-acpi/Kernel.elf=$1/sp-acpi/Kernel.elf \
	nos/sp-noacpi/Kernel.elf=$1/sp-noacpi/Kernel.elf

