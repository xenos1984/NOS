# Process this file with automake to produce a Makefile.in

AUTOMAKE_OPTIONS = nostdinc

AM_CPPFLAGS = -imacros $(top_srcdir)/config.h -I$(top_srcdir)/kernel -I$(top_srcdir)/include

noinst_LIBRARIES = libia32.a

libia32_a_SOURCES = \
	Entry.h \
	Process.h \
	I386TaskManager.h \
	Thread.h \
	PMode.h \
	Registers.h \
	Symbol.h \
	SysCall.h \
	\
	Core.cpp \
	Entry.S \
	I386TaskManager.cpp \
	InterruptStub.S \
	Registers.cpp \
	Thread.cpp \
	User.cpp

EXTRA_DIST = ldscript
