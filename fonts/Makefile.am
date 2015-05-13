# Process this file with automake to produce a Makefile.in

AUTOMAKE_OPTIONS = nostdinc

AM_CPPFLAGS = -imacros $(top_srcdir)/config.h -I$(top_srcdir)/include

noinst_LIBRARIES = libfonts.a

libfonts_a_SOURCES = \
	Font8x8.cpp \
	Font8x14.cpp \
	Font8x16.cpp \
	Font8x8t.cpp \
	Font8x14t.cpp \
	Font8x16t.cpp
