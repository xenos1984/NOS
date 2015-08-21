# Process this file with automake to produce a Makefile.in

AUTOMAKE_OPTIONS = nostdinc

AM_CPPFLAGS = -imacros $(top_srcdir)/config.h -I$(top_srcdir)/kernel -I$(top_srcdir)/include

noinst_LIBRARIES = libamd64.a

libamd64_a_SOURCES = \
	Entry.h \
	Memory.h \
	Pager.h \
	PMode.h \
	Process.h \
	Registers.h \
	Symbol.h \
	SysCall.h \
	Thread.h \
	X86_64TaskManager.h \
	\
	Core.cpp \
	Entry.S \
	InterruptStub.S \
	Pager.cpp \
	Registers.cpp \
	Thread.cpp \
	User.cpp \
	X86_64TaskManager.cpp

EXTRA_DIST = ldscript
