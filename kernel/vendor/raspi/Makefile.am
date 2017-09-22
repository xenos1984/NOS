# Process this file with automake to produce a Makefile.in

AUTOMAKE_OPTIONS = nostdinc

AM_CPPFLAGS = -imacros $(top_srcdir)/config.h -I$(top_srcdir)/kernel -I$(top_srcdir)/include

noinst_LIBRARIES = libraspi.a

libraspi_a_SOURCES = \
	GPIO.h \
	UART.h \
	\
	Console.cpp \
	Core.cpp \
	Entry.S \
	GPIO.cpp \
	UART.cpp

noinst_PROGRAMS = Kernel.elf

Kernel_elf_SOURCES =

Kernel_elf_DEPENDENCIES = kernel.lds libraspi.a ../../libkernel.a ../../arch/${arch_subdir}/lib${arch_subdir}.a #../../arch/${arch_subdir}/${subarch_subdir}/lib${subarch_subdir}.a

Kernel_elf_LDFLAGS = \
	-T kernel.lds \
	-Wl,-Map,Kernel.map \
	-Wl,-u,Entry

