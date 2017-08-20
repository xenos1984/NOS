# Process this file with automake to produce a Makefile.in

AUTOMAKE_OPTIONS = nostdinc

AM_CPPFLAGS = -imacros $(top_srcdir)/config.h -I$(top_srcdir)/kernel -I$(top_srcdir)/include

noinst_LIBRARIES = libpc.a

libpc_a_SOURCES = \
	ACPI.h \
	AML.h \
	Cmos.h \
	Console.h \
	HPET.h \
	IOApic.h \
	IOApicManager.h \
	Multiboot.h \
	PIC.h \
	PICManager.h \
	PIT.h \
	PITClock.h \
	SMP.h \
	\
	ACPI.cpp \
	AML.cpp \
	Cmos.cpp \
	Console.cpp \
	Core.cpp \
	HPET.cpp \
	IOApic.cpp \
	IOApicManager.cpp \
	Multiboot.cpp \
	PIC.cpp \
	PICManager.cpp \
	PIT.cpp \
	PITClock.cpp \
	SMP.cpp

noinst_PROGRAMS = Kernel.elf

Kernel_elf_SOURCES =

Kernel_elf_DEPENDENCIES = kernel.${subarch_subdir}.lds ../../arch/${arch_subdir}/${subarch_subdir}/lib${subarch_subdir}.a ../../arch/${arch_subdir}/lib${arch_subdir}.a ../../libkernel.a

Kernel_elf_LDFLAGS = \
	-T kernel.${subarch_subdir}.lds \
	-Wl,-Map,Kernel.map \
	-Wl,-u,Entry

