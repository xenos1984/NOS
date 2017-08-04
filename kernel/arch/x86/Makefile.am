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
	Pager.cpp \
	PIC.cpp \
	PICManager.cpp \
	PIT.cpp \
	PITClock.cpp \
	Processor.cpp \
	SMP.cpp \
	X86Pager.cpp

noinst_PROGRAMS = Kernel.elf

Kernel_elf_SOURCES =

Kernel_elf_DEPENDENCIES = ${subarch_subdir}/ldscript ${subarch_subdir}/lib${subarch_subdir}.a libx86.a ../../libkernel.a

Kernel_elf_LDFLAGS = \
	-Wl,-shared,-soname=Kernel.elf \
	-T ${subarch_subdir}/ldscript \
	-T ../../exports.lds \
	-Wl,-Map,Kernel.map \
	-Wl,-u,Entry

