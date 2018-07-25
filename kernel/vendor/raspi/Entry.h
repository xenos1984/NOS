// Entry.h - Some definitions used in the boot process.

#ifndef __VENDOR_RASPI_ENTRY_H__
#define __VENDOR_RASPI_ENTRY_H__

#define TABLE_SPLIT_BITS 1

#ifdef ELF64
#define USER_OFFSET 0xFFFFFFFF80000000
#define KERNEL_OFFSET 0xFFFFFFFFC0000000
#define PERI_OFFSET 0xFFFFFFFFCF000000
#else
#define USER_OFFSET 0x80000000
#define KERNEL_OFFSET 0xC0000000
#define PERI_OFFSET 0xCF000000
#endif

#endif

