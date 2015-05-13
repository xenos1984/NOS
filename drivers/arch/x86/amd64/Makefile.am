# Process this file with automake to produce a Makefile.in

AUTOMAKE_OPTIONS = subdir-objects

AM_CPPFLAGS = -imacros config.h -I$(top_srcdir)/drivers -I$(top_srcdir)/include

noinst_LIBRARIES = libamd64.a

libamd64_a_SOURCES = \
	Stub.S \
	../../../Driver.cpp

EXTRA_DIST = ldscript
