# Process this file with automake to produce a Makefile.in

SUBDIRS = ${subarch_subdir} .

DIST_SUBDIRS = ia32 amd64

AUTOMAKE_OPTIONS = nostdinc

AM_CPPFLAGS = -imacros $(top_srcdir)/config.h -I$(top_srcdir)/kernel -I$(top_srcdir)/include

noinst_LIBRARIES = libx86.a

libx86_a_SOURCES = \
	ACPI.h \
	AML.h \
	Apic.h \
	AtomicOps.h \
	Cmos.h \
	Console.h \
	ControlRegisters.h \
	CPU.h \
	DescriptorTable.h \
	FPU.h \
	HPET.h \
	Interrupt.h \
	IOApic.h \
	IOApicManager.h \
	Memory.h \
	MSR.h \
	Multiboot.h \
	Pager.h \
	PIC.h \
	PICManager.h \
	PIT.h \
	PITClock.h \
	Port.h \
	Process.h \
	Processor.h \
	SMP.h \
	Symbol.h \
	SysCall.h \
	Thread.h \
	X86Pager.h \
	\
	ACPI.cpp \
	AML.cpp \
	Apic.cpp \
	Cmos.cpp \
	Console.cpp \
	CPU.cpp \
	DescriptorTable.cpp \
	FPU.cpp \
	HPET.cpp \
	Interrupt.cpp \
	IOApic.cpp \
	IOApicManager.cpp \
	Multiboot.cpp \
	PIC.cpp \
	PICManager.cpp \
	PIT.cpp \
	PITClock.cpp \
	Processor.cpp \
	SMP.cpp \
	X86Pager.cpp

noinst_PROGRAMS = Kernel.elf ../../../Kernel.elf eltorito.iso NOS.iso

Kernel_elf_SOURCES =

Kernel_elf_DEPENDENCIES = ${subarch_subdir}/ldscript ${subarch_subdir}/lib${subarch_subdir}.a libx86.a ../../libkernel.a

Kernel_elf_LDFLAGS = \
	-Wl,-shared,-soname=Kernel.elf \
	-T ${subarch_subdir}/ldscript \
	-T ../../exports.lds \
	-Wl,-Map,Kernel.map

_________Kernel_elf_SOURCES =

../../../Kernel.elf$(EXEEXT): Kernel.elf
	$(NOS_OBJCOPY) $(copy_flags) $< $@

eltorito_iso_SOURCES =

eltorito.iso$(EXEEXT):
	grub-mkimage -o eltorito.img -O i386-pc-eltorito biosdisk iso9660 multiboot configfile

NOS_iso_SOURCES =

NOS.iso$(EXEEXT): Kernel.elf grub.cfg eltorito.iso
	genisoimage -R -f -c boot/boot.cat -b boot/grub/eltorito.img -no-emul-boot -boot-load-size 4 -boot-info-table -o NOS.iso -graft-points nos/Kernel.elf=Kernel.elf boot/grub/eltorito.img=eltorito.img boot/grub/grub.cfg=grub.cfg

