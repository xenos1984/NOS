# Process this file with automake to produce a Makefile.in

AUTOMAKE_OPTIONS = nostdinc

AM_CPPFLAGS = -imacros $(top_srcdir)/config.h -I$(top_srcdir)/kernel -I$(top_srcdir)/include

noinst_LIBRARIES = liba64.a

liba64_a_SOURCES = \
	Interrupt.h \
	Pager.h \
	PageTable.h \
	PageTableEntry.h \
	Symbol.h \
	SystemRegs.h \
	\
	Interrupt.cpp \
	InterruptStub.S \
	Pager.cpp
