# Process this file with automake to produce a Makefile.in

# NOS Kernel - architecture independent part.

SUBDIRS = . arch vendor

AUTOMAKE_OPTIONS = nostdinc subdir-objects

AM_CPPFLAGS = -imacros $(top_srcdir)/config.h -I$(top_srcdir)/kernel -I$(top_srcdir)/include

noinst_LIBRARIES = libkernel.a

libkernel_a_SOURCES = \
	Allocator.h \
	Chunker.h \
	Clock.h \
	Console.h \
	Core.h \
	Elf.h \
	Heap.h \
	IRQManager.h \
	Memory.h \
	Pager.h \
	Process.h \
	Processor.h \
	Segment.h \
	SpinLock.h \
	Symbol.h \
	Tasker.h \
	TaskManager.h \
	TaskScheduler.h \
	Thread.h \
	Time.h \
	\
	Allocator.cpp \
	Clock.cpp \
	Console.cpp \
	Core.cpp \
	Elf.cpp \
	IRQManager.cpp \
	Memory.cpp \
	Pager.cpp \
	Process.cpp \
	Processor.cpp \
	SpinLock.cpp \
	Tasker.cpp \
	TaskScheduler.cpp \
	Thread.cpp \
	ThreadQueue.cpp

libkernel_a_SOURCES += \
	chunker/bitmap/Chunker.h \
	chunker/bitmap/Chunker.cpp

libkernel_a_SOURCES += \
	heap/Heap.h \
	heap/Heap.cpp \
	heap/list/Heap.cpp

# libkernel_a_SOURCES += \
# 	chunker/buddy/Chunker.h \
# 	chunker/buddy/Chunker.cpp
