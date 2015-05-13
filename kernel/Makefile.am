# Process this file with automake to produce a Makefile.in

# NOS Kernel.

SUBDIRS = . arch

AUTOMAKE_OPTIONS = nostdinc

AM_CPPFLAGS = -imacros $(top_srcdir)/config.h -I$(top_srcdir)/kernel -I$(top_srcdir)/include

noinst_LIBRARIES = libkernel.a

libkernel_a_SOURCES = \
	AtomicOps.h \
	Clock.h \
	Console.h \
	Core.h \
	Elf.h \
	IRQManager.h \
	Memory.h \
	PhysicalMemory.h \
	Process.h \
	TaskManager.h \
	TaskScheduler.h \
	Thread.h \
	Time.h \
	VirtualMemory.h \
	\
	Clock.cpp \
	Console.cpp \
	Core.cpp \
	IRQManager.cpp \
	Process.cpp \
	TaskScheduler.cpp \
	Thread.cpp \
	ThreadQueue.cpp \
	VirtualMemory.cpp
