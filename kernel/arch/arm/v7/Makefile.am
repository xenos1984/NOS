# Process this file with automake to produce a Makefile.in

AUTOMAKE_OPTIONS = nostdinc

AM_CPPFLAGS = -imacros $(top_srcdir)/config.h -I$(top_srcdir)/kernel -I$(top_srcdir)/include

noinst_LIBRARIES = libv7.a

libv7_a_SOURCES = \
	Pager.h \
	PageTable.h \
	PageTableEntry.h \
	\
	Pager.cpp \
	PageTableEntry.cpp
