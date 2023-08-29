# Process this file with automake to produce a Makefile.in

AUTOMAKE_OPTIONS = nostdinc

AM_CPPFLAGS = -imacros $(top_srcdir)/config.h -I$(top_srcdir)/kernel -I$(top_srcdir)/include

noinst_LIBRARIES = libx32.a

libx32_a_SOURCES = \
	Entry.h \
	I386TaskManager.h \
	Memory.h \
	Pager.h \
	PMode.h \
	Process.h \
	Registers.h \
	Symbol.h \
	SysCall.h \
	Tasker.h \
	Thread.h \
	\
	Core.cpp \
	Entry.S \
	I386TaskManager.cpp \
	InterruptStub.S \
	Pager.cpp \
	Registers.cpp \
	Tasker.cpp \
	Thread.cpp \
	User.cpp
