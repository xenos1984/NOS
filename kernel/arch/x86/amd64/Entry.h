// Entry.h - Some definitions used in the boot process.

#ifndef __ARCH_X86_AMD64_ENTRY_H__
#define __ARCH_X86_AMD64_ENTRY_H__

#define USER_OFFSET 0xffff800000000000
#define KERNEL_OFFSET 0xffffffff80000000

#define IDT_LENGTH 0x1000
#define GDT_LENGTH 0x1000
#define TSS_LIN_ADDR 0xffffffff40000000
#define TSS_LENGTH 0x3000
#define FIRST_TSS 5
#define FIRST_IRQ 0x20
#define STACK_LIN_ADDR 0xffffffff00000000
#define STACK_SIZE 0x2000
#define STACK_SIZE_SHIFT 13

#define KERNEL_CODE 0x20
#define KERNEL_STACK 0x28
#define KERNEL_DATA 0x30
#define USER_DATA 0x3b
#define USER_STACK 0x43
#define USER_CODE 0x4b

#endif
