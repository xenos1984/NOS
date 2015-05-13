# Process this file with automake to produce a Makefile.in

AUTOMAKE_OPTIONS = nostdinc

AM_CPPFLAGS = -imacros $(top_srcdir)/config.h -I$(top_srcdir)/kernel -I$(top_srcdir)/include

noinst_LIBRARIES = libatari.a

libatari_a_SOURCES = \
	AtariConsole.h \
	Blitter.h \
	Interrupt.h \
	MFP.h \
	PSG.h \
	SysVars.h \
	Video.h \
	\
	AtariConsole.cpp \
	Blitter.cpp \
	Core.cpp \
	Interrupt.cpp \
	InterruptStub.S \
	MFP.cpp \
	PSG.cpp \
	Video.cpp

noinst_PROGRAMS = ROM.bin

ROM_bin_SOURCES = \
	ROMEntry.S

ROM_bin_DEPENDENCIES = libatari.a ../../../libkernel.a ../../../../fonts/libfonts.a ROM.lds

ROM_bin_LDFLAGS = \
	-T ROM.lds \
	-Wl,-Map,ROM.map

EXTRA_DIST = ROM.lds
