# Process this file with automake to produce a Makefile.in

SUBDIRS = ${subarch_subdir} .

DIST_SUBDIRS = v7

AUTOMAKE_OPTIONS = nostdinc

AM_CPPFLAGS = -imacros $(top_srcdir)/config.h -I$(top_srcdir)/kernel -I$(top_srcdir)/include

noinst_LIBRARIES = libarm.a

libarm_a_SOURCES = \
	AtomicOps.h \
	Coprocessor.h \
	Delay.h \
	Extensions.h \
	Interrupt.h \
	Memory.h \
	Pager.h \
	Process.h \
	Symbol.h \
	Thread.h \
	\
	Interrupt.cpp \
	InterruptStub.S

