AC_DEFUN([NOS_SET_HOST], [
	AH_TEMPLATE([ELF32], [Define to 1 for 32 bit ELF targets.])
	AH_TEMPLATE([ELF64], [Define to 1 for 64 bit ELF targets.])
	AH_TEMPLATE([uintX_t], [Unsigned data type.])
	AH_TEMPLATE([intX_t], [Signed data type.])
	case "${host_cpu}" in
	i*86)
		arch_subdir=x86
		subarch_subdir=ia32
		host_bfd=elf32-i386
		copy_flags="-I ${host_bfd} -O ${host_bfd}"
		CFLAGS="${CFLAGS} -mno-sse -mno-mmx -mno-sse2 -mno-3dnow -mno-avx -mno-80387"
		AC_DEFINE([ELF32])
		AC_DEFINE([uintX_t], [uint32_t])
		AC_DEFINE([intX_t], [int32_t])
		AC_DEFINE([ARCH_X86_IA32], [1], [Define to 1 for IA32 targets.])
		;;
	x86_64)
		arch_subdir=x86
		subarch_subdir=amd64
		host_bfd=elf64-x86-64
		copy_flags="-I ${host_bfd} -O ${host_bfd}"
		CFLAGS="${CFLAGS} -mcmodel=large -mno-red-zone -mno-sse -mno-mmx -mno-sse2 -mno-3dnow -mno-avx -mno-80387"
		LDFLAGS="${LDFLAGS} -Wl,-z -Wl,max-page-size=0x1000"
		AC_DEFINE([ELF64])
		AC_DEFINE([uintX_t], [uint64_t])
		AC_DEFINE([intX_t], [int64_t])
		AC_DEFINE([ARCH_X86_AMD64], [1], [Define to 1 for AMD64 targets.])
		;;
	m68k)
		arch_subdir=m68k
		host_bfd=elf32-m68k
		copy_flags="-I ${host_bfd} -O ${host_bfd}"
		AC_DEFINE([ELF32])
		AC_DEFINE([uintX_t], [uint32_t])
		AC_DEFINE([intX_t], [int32_t])
		case "${host_vendor}" in
		atari)
			subarch_subdir=atari
			CFLAGS="${CFLAGS} -mcpu=68000 -lgcc"
			AC_DEFINE([ARCH_M68K_ATARI], [1], [Define to 1 for Atari targets.])
			;;
		amiga)
			subarch_subdir=amiga
			CFLAGS="${CFLAGS} -mcpu=68000 -lgcc"
			AC_DEFINE([ARCH_M68K_AMIGA], [1], [Define to 1 for Amiga targets.])
			;;
		*)
			AC_MSG_ERROR([unsupported host vendor])
			;;
		esac
		;;
	*)
		AC_MSG_ERROR([unsupported host CPU])
		;;
	esac
	AC_DEFINE_UNQUOTED([ARCH], [${arch_subdir}], [NOS target architecture.])
	AC_DEFINE_UNQUOTED([INC_ARCH(x)], [<arch/${arch_subdir}/x>], [NOS target architecture.])
	AC_DEFINE_UNQUOTED([SUBARCH], [${subarch_subdir}], [NOS target subarchitecture.])
	AC_DEFINE_UNQUOTED([INC_SUBARCH(x)], [<arch/${arch_subdir}/${subarch_subdir}/x>], [NOS target subarchitecture.])
	AC_SUBST(arch_subdir)
	AC_SUBST(subarch_subdir)
	AC_SUBST(host_bfd)
	AC_SUBST(copy_flags)
])

AC_DEFUN([NOS_SET_FLAGS], [
	CFLAGS="${CFLAGS} -ffreestanding -fno-leading-underscore -Wall -Wextra -Wpedantic"
	CXXFLAGS="${CFLAGS} -fno-rtti -fno-exceptions -std=c++14"
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
