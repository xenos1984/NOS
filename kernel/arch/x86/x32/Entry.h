// Entry.h - Some definitions used in the boot process.

#ifndef __ARCH_X86_X32_ENTRY_H__
#define __ARCH_X86_X32_ENTRY_H__

#define USER_OFFSET 0x80000000
#define KERNEL_OFFSET 0xC0000000

#define IDT_LENGTH 0x800
#define GDT_LENGTH 0x800
#define TSS_LIN_ADDR 0xff000000
#define TSS_LENGTH 0x3000
#define FIRST_TSS 8
#define FIRST_IRQ 0x20
#define STACK_LIN_ADDR 0xf0000000
#define STACK_SIZE 0x1000
#define STACK_SIZE_SHIFT 12

#define KERNEL_DATA 0x08
#define KERNEL_CODE 0x10
#define KERNEL_STACK 0x18
#define USER_DATA 0x23
#define USER_CODE 0x2b
#define USER_STACK 0x33

#endif
