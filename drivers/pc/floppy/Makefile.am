# Process this file with automake to produce a Makefile.in

AM_CPPFLAGS = -imacros config.h -I$(top_srcdir)/drivers -I$(top_srcdir)/include

noinst_PROGRAMS = floppy ../../../Floppy.elf

floppy_SOURCES = \
	FloppyDriver.cpp

floppy_DEPENDENCIES = \
	$(top_srcdir)/drivers/arch/$(arch_subdir)/ldscript

floppy_LDFLAGS = \
	-T $(top_srcdir)/drivers/arch/$(arch_subdir)/ldscript

_________Floppy_elf_SOURCES =

../../../Floppy.elf$(EXEEXT): floppy
#	cp $< $@
	$(NOS_OBJCOPY) $(copy_flags) $< $@
