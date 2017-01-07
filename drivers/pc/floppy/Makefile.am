# Process this file with automake to produce a Makefile.in

AM_CPPFLAGS = -imacros config.h -I$(top_srcdir)/drivers -I$(top_srcdir)/include

noinst_PROGRAMS = Floppy.elf

Floppy_elf_SOURCES = \
	FloppyDriver.cpp

Floppy_elf_DEPENDENCIES = \
	$(top_srcdir)/drivers/arch/$(arch_subdir)/ldscript

Floppy_elf_LDFLAGS = \
	-T $(top_srcdir)/drivers/arch/$(arch_subdir)/ldscript

