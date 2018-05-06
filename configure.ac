# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.57)
AC_INIT([NOS], [0.1], [xenos1984@sourceforge.net])

AC_CONFIG_SRCDIR([kernel/Core.cpp])
AC_CONFIG_AUX_DIR([config])
AC_CONFIG_MACRO_DIR([m4])

AC_CANONICAL_HOST
AM_INIT_AUTOMAKE([foreign 1.7])
AC_CONFIG_HEADERS([config.h])

# Set target and flags.
NOS_SET_HOST

AC_ARG_ENABLE([debug],
	AC_HELP_STRING([--enable-debug], [generate debug symbols (default is NO)]),
	[CFLAGS="${CFLAGS} -g"]
)

AC_ARG_ENABLE([opt],
	AC_HELP_STRING([--disable-opt], [optimize code (default is YES)]),
	[case "${enableval}" in
	no | 0)
		CFLAGS="${CFLAGS} -O0"
		;;
	1)
		CFLAGS="${CFLAGS} -O1"
		;;
	2)
		CFLAGS="${CFLAGS} -O2"
		;;
	3)
		CFLAGS="${CFLAGS} -O3"
		;;
	*)
		CFLAGS="${CFLAGS} -O2"
		;;
	esac],
	[CFLAGS="${CFLAGS} -O2"]
)

NOS_SET_FLAGS

# Kernel config options.
AH_TEMPLATE([CONFIG_ACPI], [Define to 1 if ACPI support should be included.])
AC_ARG_ENABLE([acpi],
	AC_HELP_STRING([--disable-acpi], [disable ACPI support]),
	[if test "$enableval" != "no"; then
		AC_DEFINE([CONFIG_ACPI])
	fi],
	[AC_DEFINE([CONFIG_ACPI])]
)

AH_TEMPLATE([CONFIG_SMP], [Define to 1 if SMP support should be included.])
AC_ARG_ENABLE([smp],
	AC_HELP_STRING([--disable-smp], [disable SMP support]),
	[if test "$enableval" != "no"; then
		AC_DEFINE([CONFIG_SMP])
	fi],
	[AC_DEFINE([CONFIG_SMP])]
)

# Checks for programs.
AC_PROG_CXX
AC_PROG_RANLIB
AC_PROG_LN_S
AM_PROG_AS
NOS_PROG_OBJCOPY

# Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_C_INLINE
AC_C_VOLATILE

AC_TYPE_SIZE_T

# Some compiler dependent features.
AC_DEFINE_UNQUOTED([PACKED], [__attribute__((packed))], [Keyword for packing structures.])
AC_DEFINE_UNQUOTED([SECTION(x)], [__attribute__((section(x)))], [Keyword for section placement.])

# Generate output files.
AC_CONFIG_FILES([
	Makefile
	fonts/Makefile
	kernel/Makefile
	kernel/arch/Makefile
	kernel/arch/m68k/Makefile
	kernel/arch/m68k/atari/Makefile
	kernel/arch/arm/Makefile
	kernel/arch/arm/a32/Makefile
	kernel/arch/arm/a64/Makefile
	kernel/arch/arm/v7/Makefile
	kernel/arch/arm/v8/Makefile
	kernel/arch/x86/Makefile
	kernel/arch/x86/ia32/Makefile
	kernel/arch/x86/amd64/Makefile
	kernel/vendor/Makefile
	kernel/vendor/pc/Makefile
	kernel/vendor/raspi/Makefile
	include/Makefile
	include/arch/Makefile
	include/arch/x86/Makefile
	include/arch/x86/ia32/Makefile
	include/arch/x86/amd64/Makefile
	include/arch/m68k/Makefile
	drivers/Makefile
	drivers/arch/Makefile
	drivers/arch/x86/Makefile
	drivers/arch/x86/ia32/Makefile
	drivers/arch/x86/amd64/Makefile
	drivers/arch/arm/Makefile
	drivers/arch/m68k/Makefile
	drivers/pc/Makefile
	drivers/pc/floppy/Makefile
	drivers/raspi/Makefile
	drivers/atari/Makefile
	images/Makefile
	images/pc/Makefile
	images/raspi/Makefile
	refman/Makefile
	refman/refman.doxyfile
])
AC_OUTPUT
