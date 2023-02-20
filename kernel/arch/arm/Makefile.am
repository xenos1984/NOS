# Process this file with automake to produce a Makefile.in

SUBDIRS = ${subarch_subdir} ${bits_subdir} .

DIST_SUBDIRS = a32 a64 v7 v8

AUTOMAKE_OPTIONS = nostdinc

AM_CPPFLAGS = -imacros $(top_srcdir)/config.h -I$(top_srcdir)/kernel -I$(top_srcdir)/include

noinst_LIBRARIES = libarm.a

libarm_a_SOURCES = \
	Delay.h \
	Extensions.h \
	Memory.h \
	Pager.h \
	Process.h \
	Symbol.h \
	Thread.h
