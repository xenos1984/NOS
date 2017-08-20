# Process this file with automake to produce a Makefile.in

SUBDIRS = ${subarch_subdir} .

DIST_SUBDIRS = ia32 amd64

AUTOMAKE_OPTIONS = nostdinc

AM_CPPFLAGS = -imacros $(top_srcdir)/config.h -I$(top_srcdir)/kernel -I$(top_srcdir)/include

noinst_LIBRARIES = libx86.a

libx86_a_SOURCES = \
	Apic.h \
	AtomicOps.h \
	ControlRegisters.h \
	CPU.h \
	DescriptorTable.h \
	FPU.h \
	Interrupt.h \
	Memory.h \
	MSR.h \
	Pager.h \
	PageTable.h \
	PageTableEntry.h \
	Port.h \
	Process.h \
	Processor.h \
	Symbol.h \
	SysCall.h \
	Thread.h \
	\
	Apic.cpp \
	CPU.cpp \
	DescriptorTable.cpp \
	FPU.cpp \
	Interrupt.cpp \
	Pager.cpp \
	Processor.cpp

#noinst_PROGRAMS = Kernel.elf

#Kernel_elf_SOURCES =

#Kernel_elf_DEPENDENCIES = ${subarch_subdir}/ldscript ${subarch_subdir}/lib${subarch_subdir}.a libx86.a ../../libkernel.a

#Kernel_elf_LDFLAGS = \
#	-Wl,-shared,-soname=Kernel.elf \
#	-T ${subarch_subdir}/ldscript \
#	-T ../../exports.lds \
#	-Wl,-Map,Kernel.map \
#	-Wl,-u,Entry

