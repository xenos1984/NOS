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
	ControlRegisters.h \
	CPU.h \
	DescriptorTable.h \
	FPU.h \
	HPET.h \
	Interrupt.h \
	IOApic.h \
	IOApicManager.h \
	MSR.h \
	Multiboot.h \
	PIC.h \
	PICManager.h \
	PIT.h \
	PITClock.h \
	Port.h \
	Process.h \
	Processor.h \
	SMP.h \
	SysCall.h \
	Thread.h \
	VGAConsole.h \
	X86Pager.h \
	\
	ACPI.cpp \
	AML.cpp \
	Apic.cpp \
	Cmos.cpp \
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
	VGAConsole.cpp \
	X86Pager.cpp

noinst_PROGRAMS = Kernel.elf ../../../Kernel.elf

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
