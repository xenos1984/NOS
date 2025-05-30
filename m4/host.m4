AC_DEFUN([NOS_SET_HOST], [
	AH_TEMPLATE([ELF32], [Define to 1 for 32 bit ELF targets.])
	AH_TEMPLATE([ELF64], [Define to 1 for 64 bit ELF targets.])
	AH_TEMPLATE([uintX_t], [Unsigned data type.])
	AH_TEMPLATE([intX_t], [Signed data type.])
	AH_TEMPLATE([CPU_COUNT], [CPU count: 0 = variable, > 0 for fixed number.])
	case "${host_cpu}" in
	i*86)
		arch_subdir=x86
		bits_subdir=x32
		host_bfd=elf32-i386
		copy_flags="-I ${host_bfd} -O ${host_bfd}"
		ac_tool_prefix="${host_cpu}-elf-"
		CFLAGS="${CFLAGS} -mgeneral-regs-only"
		AC_DEFINE([ELF32])
		AC_DEFINE([uintX_t], [uint32_t])
		AC_DEFINE([intX_t], [int32_t])
		AC_DEFINE([ARCH_X86], [1], [Define to 1 for X86 targets.])
		AC_DEFINE([ARCH_X86_X32], [1], [Define to 1 for IA32 targets.])
		case "${host_vendor}" in
		pc)
			platform_subdir=pc
			AC_DEFINE([CPU_COUNT], [0])
			;;
		*)
			AC_MSG_ERROR([unsupported host platform])
			;;
		esac
		;;
	x86_64)
		arch_subdir=x86
		bits_subdir=x64
		host_bfd=elf64-x86-64
		copy_flags="-I ${host_bfd} -O ${host_bfd}"
		ac_tool_prefix="x86_64-elf-"
		CFLAGS="${CFLAGS} -mcmodel=kernel -mno-red-zone -mgeneral-regs-only"
		LDFLAGS="${LDFLAGS} -Wl,-z -Wl,max-page-size=0x1000"
		AC_DEFINE([ELF64])
		AC_DEFINE([uintX_t], [uint64_t])
		AC_DEFINE([intX_t], [int64_t])
		AC_DEFINE([ARCH_X86], [1], [Define to 1 for X86 targets.])
		AC_DEFINE([ARCH_X86_X64], [1], [Define to 1 for AMD64 targets.])
		case "${host_vendor}" in
		pc)
			platform_subdir=pc
			AC_DEFINE([CPU_COUNT], [0])
			;;
		*)
			AC_MSG_ERROR([unsupported host platform])
			;;
		esac
		;;
	arm)
		arch_subdir=arm
		bits_subdir=a32
		host_bfd=elf32-littlearm
		copy_flags="-I ${host_bfd} -O ${host_bfd}"
		AC_DEFINE([ELF32])
		AC_DEFINE([uintX_t], [uint32_t])
		AC_DEFINE([intX_t], [int32_t])
		AC_DEFINE([ARCH_ARM], [1], [Define to 1 for ARM targets.])
		case "${host_vendor}" in
		raspiz|raspizw|raspi1ap|raspi1bp|raspi1)
			CFLAGS="${CFLAGS} -mcpu=arm1176jzf-s -mgeneral-regs-only"
			subarch_subdir=v6
			platform_subdir=raspi
			ac_tool_prefix="arm-eabi-"
			AC_DEFINE([ARCH_ARM_V6], [1], [Define to 1 for ARMv6 targets.])
			AC_DEFINE([ARCH_ARM_1176JZFS], [1], [Define to 1 for ARM 1176JZF-S targets.])
			AC_DEFINE([CPU_COUNT], [1])
			;;
		raspi2b|raspi2)
			CFLAGS="${CFLAGS} -mcpu=cortex-a7 -mgeneral-regs-only"
			subarch_subdir=v7
			platform_subdir=raspi
			ac_tool_prefix="arm-eabihf-"
			AC_DEFINE([ARCH_ARM_V7], [1], [Define to 1 for ARMv7 targets.])
			AC_DEFINE([ARCH_ARM_CORTEX_A7], [1], [Define to 1 for ARM Cortex-A7 targets.])
			AC_DEFINE([CPU_COUNT], [4])
			;;
		*)
			AC_MSG_ERROR([unsupported host platform])
			;;
		esac
		AC_DEFINE_UNQUOTED([SUBARCH], [${subarch_subdir}], [NOS target subarchitecture.])
		AC_DEFINE_UNQUOTED([INC_SUBARCH(x)], [<arch/${arch_subdir}/${subarch_subdir}/x>], [NOS target subarchitecture.])
		AC_SUBST(subarch_subdir)
		;;
	aarch64)
		arch_subdir=arm
		bits_subdir=a64
		host_bfd=elf64-littleaarch64
		copy_flags="-I ${host_bfd} -O ${host_bfd}"
		ac_tool_prefix="aarch64-elf-"
		AC_DEFINE([ELF64])
		AC_DEFINE([uintX_t], [uint64_t])
		AC_DEFINE([intX_t], [int64_t])
		AC_DEFINE([ARCH_ARM], [1], [Define to 1 for ARM targets.])
		case "${host_vendor}" in
		raspi3ap|raspi3b|raspi3bp|raspiz2w|raspi3)
			CFLAGS="${CFLAGS} -mcpu=cortex-a53 -mgeneral-regs-only"
			subarch_subdir=v8
			platform_subdir=raspi
			AC_DEFINE([ARCH_ARM_V8], [1], [Define to 1 for ARMv8 targets.])
			AC_DEFINE([ARCH_ARM_CORTEX_A53], [1], [Define to 1 for ARM Cortex-A53 targets.])
			AC_DEFINE([CPU_COUNT], [4])
			;;
		raspi4b|raspi4)
			CFLAGS="${CFLAGS} -mcpu=cortex-a72 -mgeneral-regs-only"
			subarch_subdir=v8
			platform_subdir=raspi
			AC_DEFINE([ARCH_ARM_V8], [1], [Define to 1 for ARMv8 targets.])
			AC_DEFINE([ARCH_ARM_CORTEX_A72], [1], [Define to 1 for ARM Cortex-A72 targets.])
			AC_DEFINE([CPU_COUNT], [4])
			;;
		*)
			AC_MSG_ERROR([unsupported host platform])
			;;
		esac
		AC_DEFINE_UNQUOTED([SUBARCH], [${subarch_subdir}], [NOS target subarchitecture.])
		AC_DEFINE_UNQUOTED([INC_SUBARCH(x)], [<arch/${arch_subdir}/${subarch_subdir}/x>], [NOS target subarchitecture.])
		AC_SUBST(subarch_subdir)
		;;
	m68k)
		arch_subdir=m68k
		host_bfd=elf32-m68k
		copy_flags="-I ${host_bfd} -O ${host_bfd}"
		ac_tool_prefix="m68k-elf-"
		AC_DEFINE([ELF32])
		AC_DEFINE([uintX_t], [uint32_t])
		AC_DEFINE([intX_t], [int32_t])
		case "${host_vendor}" in
		atari)
			subarch_subdir=atari
			CFLAGS="${CFLAGS} -mcpu=68000 -lgcc"
			AC_DEFINE([ARCH_M68K_ATARI], [1], [Define to 1 for Atari targets.])
			AC_DEFINE([CPU_COUNT], [1])
			;;
		amiga)
			subarch_subdir=amiga
			CFLAGS="${CFLAGS} -mcpu=68000 -lgcc"
			AC_DEFINE([ARCH_M68K_AMIGA], [1], [Define to 1 for Amiga targets.])
			AC_DEFINE([CPU_COUNT], [1])
			;;
		*)
			AC_MSG_ERROR([unsupported host platform])
			;;
		esac
		;;
	*)
		AC_MSG_ERROR([unsupported host CPU])
		;;
	esac
	AC_DEFINE_UNQUOTED([ARCH], [${arch_subdir}], [NOS target architecture.])
	AC_DEFINE_UNQUOTED([INC_ARCH(x)], [<arch/${arch_subdir}/x>], [NOS target architecture.])
	AC_DEFINE_UNQUOTED([BITS], [${bits_subdir}], [NOS target bit subtype.])
	AC_DEFINE_UNQUOTED([INC_BITS(x)], [<arch/${arch_subdir}/${bits_subdir}/x>], [NOS target bit subtype.])
	AC_DEFINE_UNQUOTED([PLATFORM], [${platform_subdir}], [NOS target platform.])
	AC_DEFINE_UNQUOTED([INC_PLATFORM(x)], [<platform/${platform_subdir}/x>], [NOS target platform.])
	AC_SUBST(arch_subdir)
	AC_SUBST(bits_subdir)
	AC_SUBST(platform_subdir)
	AC_SUBST(host_bfd)
	AC_SUBST(copy_flags)
	AC_SUBST(ac_tool_prefix)
])

AC_DEFUN([NOS_SET_FLAGS], [
	CFLAGS="${CFLAGS} -ffreestanding -fno-leading-underscore -Wall -Wextra -Wpedantic"
	CXXFLAGS="${CFLAGS} -fno-rtti -fno-exceptions -std=c++23"
	LDFLAGS="${LDFLAGS} -nostartfiles -nostdlib -fno-rtti -fno-exceptions"
])

AC_DEFUN([NOS_PROG_OBJCOPY], [
	AC_CHECK_TOOL([NOS_OBJCOPY], [objcopy])
	AC_CACHE_CHECK([whether objcopy generates $host_bfd],
		[ac_cv_objcopy_supports_host_bfd],
		[if test "$NOS_OBJCOPY" --info 2>&1 < /dev/null | grep "$host_bfd" > /dev/null; then
			ac_cv_objcopy_supports_host_bfd=no
		else
			ac_cv_objcopy_supports_host_bfd=yes
		fi]
	)
	if test ac_cv_objcopy_supports_host_bfd = no; then
		AC_MSG_ERROR([unsupported host BFD])
	fi
])
