// Entry.S - Entry point of the kernel.

.extern KernelEntry
.extern FreeMemory
.extern initStart
.extern dataEnd
.extern kernelEnd
.extern bspStack

#if defined CONFIG_SMP || defined CONFIG_ACPI
.extern ApplicationEntry
#endif

#include <arch/x86/ia32/Entry.h>

.section .init.text
.code32

.global Entry
.type Entry, @function
Entry:
	// make sure we're going in the right direction!
	cld
	cli

	// create page tables, first clear them
	pushl %eax
	xorl %eax, %eax
	movl $(tabPGDIR - KERNEL_OFFSET), %edi
	movl $0x2000, %ecx
	rep stosb

	// chain tables together
	movl $(tabPGTAB - KERNEL_OFFSET + 3), (tabPGDIR - KERNEL_OFFSET)
	movl $(tabPGTAB - KERNEL_OFFSET + 3), (tabPGDIR - KERNEL_OFFSET + (KERNEL_OFFSET >> 20))

	// enter page directory as last page table, causing a chain reaction to mirror the tables at the top of virtual memory
	movl $(tabPGDIR - KERNEL_OFFSET + 3), (tabPGDIR - KERNEL_OFFSET + 0x0ffc)

	// fill page table to map the first 4MB to KERNEL_OFFSET
	movl $0x400, %ecx
	movl $0x00000003, %eax
	movl $(tabPGTAB - KERNEL_OFFSET), %edi
1:
	stosl
	addl $0x1000, %eax
	loop 1b

	// set page dir base
	movl $(tabPGDIR - KERNEL_OFFSET), %eax
	movl %eax, %cr3

	// enable paging, enable floating point exception
	movl %cr0, %ecx
	orl $0x8000002a, %ecx
	movl %ecx, %cr0

	// clear the idt + gdt @ tabIDT
	xorl %eax, %eax
	movl $tabIDT, %edi
	movl $(IDT_LENGTH + GDT_LENGTH), %ecx
	rep stosb
	popl %eax

	// setup a bare bones GDT
	movl $myGDT, %esi
	movl $tabGDT, %edi
	movl $(8 * 8), %ecx
	rep movsb

	lgdt pGDT // load the GDT
	lidt pIDT // load the IDT
	lldt pLDT // load the LDT (0)

	movw $KERNEL_DATA, %dx
	movw %dx, %ds
	movw %dx, %es
	movw %dx, %fs
	movw %dx, %gs
	movw $KERNEL_STACK, %dx
	movw %dx, %ss

	// load cs with new selector
	ljmp $KERNEL_CODE, $FlushGDT
.size Entry, . - Entry

.section .text
.type FlushGDT, @function
FlushGDT:
	// clear identity mapping, preserve only kernel area
	movl $0, (0xfffff000)

	// set up kernel stack
	movl $bspStack, %esp

	// push the multiboot info structure, and magic
//	addl $KERNEL_OFFSET, %ebx
	pushl %ebx
	pushl %eax
	// time for some C!
	call KernelEntry // run the boot module by calling its C++ entry point
	addl $8, %esp
	call FreeMemory
	pushl $0
	str %eax
	shrl $3, %eax
	subl $FIRST_TSS, %eax
	movl $TSS_LENGTH, %ebx
	mull %ebx
	addl $TSS_LIN_ADDR, %eax
	movl $KERNEL_STACK, 8(%eax)
	movl %esp, 4(%eax)

1:
	sti
	hlt
	jmp 1b
.size FlushGDT, . - FlushGDT

#if defined CONFIG_SMP || defined CONFIG_ACPI
.type ap_in_pmode, @function
ap_in_pmode:
	movl $(tabPGTAB - KERNEL_OFFSET + 3), (tabPGDIR - KERNEL_OFFSET) // temporarily restore identity mapping
	movl $(tabPGDIR - KERNEL_OFFSET), %eax
	movl %eax, %cr3

	// enable paging, enable floating point exception
	movl %cr0, %ecx
	orl $0x8000002a, %ecx
	movl %ecx, %cr0

	lgdt pGDT
	lidt pIDT
	lldt pLDT

	ljmpl $KERNEL_CODE, $1f

1:
	movl $0, (0xfffff000) // clear identity mapping
	movl apstack, %esp
	call ApplicationEntry // run the boot module by calling its C++ entry point
	pushl $0
	str %eax
	shrl $3, %eax
	subl $FIRST_TSS, %eax
	movl $TSS_LENGTH, %ebx
	mull %ebx
	addl $TSS_LIN_ADDR, %eax
	movl $KERNEL_STACK, 8(%eax)
	movl %esp, 4(%eax)
2:
	sti
	hlt
	jmp 2b

.section .init.text
.code16 // APs start here in real mode (this (self-relocating) code is moved below 1MB) with IP = 0x0000
.global apentry
.type apentry, @function
apentry:
	cli
	movw %cs, %ax
	movw %ax, %ds
	movl %cr0, %eax
	btsl $0, %eax // enable pmode
	movl %eax, %cr0
	lgdt (pGDT - apentry)
	movw $KERNEL_DATA, %dx // 0x08 - kernel data segment
	movw %dx, %ds
	movw %dx, %es
	movw %dx, %fs
	movw %dx, %gs
	movw $KERNEL_STACK, %dx // 0x18 - kernel stack segment
	movw %dx, %ss
	ljmpl $KERNEL_CODE, $(ap_in_pmode - KERNEL_OFFSET)
.size apentry, . - apentry

#endif

// need them here since APs can't access .data from real mode

.type pIDT, @object
pIDT: .word IDT_LENGTH   // limit of 256 IDT slots
      .long tabIDT       // starting at tabIDT
.size pIDT, . - pIDT

.type pGDT, @object
pGDT: .word GDT_LENGTH   // 256 GDT slots
      .long tabGDT       // starting at tabGDT (after IDT)
.size pGDT, . - pGDT

.type pLDT, @object
pLDT: .word 0            // no LDT at this time, but must still load
      .long 0            // with someone (else multitasking might fail)
.size pLDT, . - pLDT

.section .init.data

.align 8

.type myGDT, @object
myGDT:
	// Null descriptor
	// base : 0x00000000
	// limit: 0x00000000 (0 GB)
	.long 0
	.long 0

	// 0x08 descriptor - Kernel data segment
	// base : 0x00000000
	// limit: 0xfffff pages (4 GB)
	// DPL  : 0
	// 32 bit, present, system, expand-up, writeable
	.long 0x0000ffff
	.long 0x00cf9200

	// 0x10 descriptor - Kernel code segment
	// base : 0x00000000
	// limit: 0xfffff (4 GB)
	// DPL  : 0
	// 32 bit, present, system, non-conforming, readable
	.long 0x0000ffff
	.long 0x00cf9a00

	// 0x18 descriptor - Kernel stack segment
	// base : 0x00000000
	// limit: 0xfffff (4 GB)
	// DPL  : 0
	// 32 bit, present, system, expand-up, writeable
	.long 0x0000ffff
	.long 0x00cf9200
	//.long 0x00cb9200

	// 0x20 descriptor - User data segment
	// base : 0x00000000
	// limit: 0xfffff pages (4 GB)
	// DPL  : 3
	// 32 bit, present, system, expand-up, writeable
	.long 0x0000ffff
	.long 0x00cff200

	// 0x28 descriptor - User code segment
	// base : 0x00000000
	// limit: 0xfffff (4 GB)
	// DPL  : 3
	// 32 bit, present, system, non-conforming, readable
	.long 0x0000ffff
	.long 0x00cffa00

	// 0x30 descriptor - User stack segment
	// base : 0x00000000
	// limit: 0xfffff (4 GB)
	// DPL  : 3
	// 32 bit, present, system, expand-up, writeable
	.long 0x0000ffff
	.long 0x00cff200
.size myGDT, . - myGDT

.global apstack
.type apstack, @object
apstack: .long 0
.size apstack, . - apstack
