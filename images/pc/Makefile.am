# Process this file with automake to produce a Makefile.in

noinst_PROGRAMS = NOS.iso

eltorito.img:
	grub-mkimage -o eltorito.img -O i386-pc-eltorito biosdisk iso9660 multiboot configfile

NOS_iso_SOURCES =

NOS.iso$(EXEEXT): grub.cfg eltorito.img \
	${top_builddir}/kernel/vendor/pc/Kernel.elf
	genisoimage -R -c boot/boot.cat -b boot/grub/eltorito.img -no-emul-boot -boot-load-size 4 -boot-info-table -o NOS.iso -graft-points \
		boot/grub/eltorito.img=eltorito.img \
		boot/grub/grub.cfg=grub.cfg \
		nos/Kernel.elf=${top_builddir}/kernel/vendor/pc/Kernel.elf
#		nos/Floppy.elf=${top_builddir}/drivers/pc/floppy/Floppy.elf
#	${top_builddir}/drivers/pc/floppy/Floppy.elf

