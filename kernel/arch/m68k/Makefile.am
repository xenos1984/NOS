# Process this file with automake to produce a Makefile.in

SUBDIRS = . ${subarch_subdir}

DIST_SUBDIRS = atari

AUTOMAKE_OPTIONS = nostdinc

AM_CPPFLAGS = -imacros $(top_srcdir)/config.h -I$(top_srcdir)/kernel -I$(top_srcdir)/include

noinst_LIBRARIES = libm68k.a

libm68k_a_SOURCES = \
	AtomicOps.h \
	Console.h \
	CPU.h \
	Process.h \
	SysCall.h \
	Thread.h \
	\
	Thread.cpp
