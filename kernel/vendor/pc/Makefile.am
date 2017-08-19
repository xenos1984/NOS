# Process this file with automake to produce a Makefile.in

AUTOMAKE_OPTIONS = nostdinc

AM_CPPFLAGS = -imacros $(top_srcdir)/config.h -I$(top_srcdir)/kernel -I$(top_srcdir)/include

noinst_LIBRARIES = libpc.a

libpc_a_SOURCES = \
	Core.cpp

noinst_PROGRAMS = Kernel.elf

Kernel_elf_SOURCES =

Kernel_elf_DEPENDENCIES = kernel.${subarch_subdir}.lds ../../arch/${arch_subdir}/${subarch_subdir}/lib${subarch_subdir}.a ../../arch/${arch_subdir}/lib${arch_subdir}.a ../../libkernel.a

Kernel_elf_LDFLAGS = \
	-T kernel.${subarch_subdir}.lds \
	-Wl,-Map,Kernel.map \
	-Wl,-u,Entry

