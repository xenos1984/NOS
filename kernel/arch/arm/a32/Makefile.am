# Process this file with automake to produce a Makefile.in

AUTOMAKE_OPTIONS = nostdinc

AM_CPPFLAGS = -imacros $(top_srcdir)/config.h -I$(top_srcdir)/kernel -I$(top_srcdir)/include

noinst_LIBRARIES = liba32.a

liba32_a_SOURCES = \
	Interrupt.h \
	Symbol.h \
	SystemRegs.h \
	\
	Interrupt.cpp \
	InterruptStub.S
