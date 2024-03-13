# Process this file with automake to produce a Makefile.in

AUTOMAKE_OPTIONS = nostdinc

AM_CPPFLAGS = -imacros $(top_srcdir)/config.h -I$(top_srcdir)/kernel -I$(top_srcdir)/include

noinst_LIBRARIES = libraspi.a

libraspi_a_SOURCES = \
	GPIO.h \
	IRQ.h \
	Mailbox.h \
	Timer.h \
	UART.h \
	\
	Console.cpp \
	Core.cpp \
	GPIO.cpp \
	Mailbox.cpp \
	UART.cpp

noinst_PROGRAMS = Kernel.elf kernel.img

Kernel_elf_SOURCES =

EXTRA_Kernel_elf_SOURCES = \
	Entry.a32.S \
	Entry.a64.S

Kernel_elf_LDADD = Entry.${bits_subdir}.$(OBJEXT)

#Kernel_elf_DEPENDENCIES = kernel.${bits_subdir}.lds Entry.${bits_subdir}.$(OBJEXT) libraspi.a ../../libkernel.a ../../arch/${arch_subdir}/lib${arch_subdir}.a ../../arch/${arch_subdir}/${bits_subdir}/lib${bits_subdir}.a ../../arch/${arch_subdir}/${subarch_subdir}/lib${subarch_subdir}.a
Kernel_elf_DEPENDENCIES = kernel.${bits_subdir}.lds Entry.${bits_subdir}.$(OBJEXT) libraspi.a ../../libkernel.a ../../arch/${arch_subdir}/lib${arch_subdir}.a ../../arch/${arch_subdir}/${bits_subdir}/lib${bits_subdir}.a

Kernel_elf_LDFLAGS = \
	-T kernel.${bits_subdir}.lds \
	-Wl,-Map,Kernel.map \
	-Wl,-u,Entry

kernel_img_SOURCES =

kernel.img$(EXEEXT): Kernel.elf
	${NOS_OBJCOPY} -O binary Kernel.elf kernel.img
