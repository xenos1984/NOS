// Entry.S - Entry point of the kernel.
// vim:set ft=asm-gnu-x86_32:

.extern KernelEntry
.extern FreeMemory
.extern initStart
.extern initTextEnd
.extern initDataStart
.extern initDataEnd
.extern textStart
.extern textEnd
.extern dataStart
.extern dataEnd
.extern kernelEnd
.extern bspStack

#if defined CONFIG_SMP || defined CONFIG_ACPI
.extern ApplicationEntry
#endif

#include <arch/x86/ia32/Entry.h>

.macro mappages from to flags
	movl $((\to - \from) >> 12), %ecx
	movl $(\from | \flags), %eax
	movl $(tabPGTAB - KERNEL_OFFSET + (\from >> 10)), %edi
1:
	stosl
	addl $0x1000, %eax
	loop 1b
.endm

.macro mappages2 from to flags
	movl $\to, %ecx
	subl $\from, %ecx
	shrl $12, %ecx
	movl $\from, %eax
	movl %eax, %edi
	orl $\flags, %eax
	shrl $10, %edi
	addl $(tabPGTAB - KERNEL_OFFSET), %edi
1:
	stosl
	addl $0x1000, %eax
	loop 1b
.endm

.section .init.text
.code32

.global Entry
.type Entry, @function
Entry:
	// Clear direction flag -> go forward.
	cld
	cli

	// Save EAX for later.
	movl %eax, %ebp

	// Zero out page tables.
	xorl %eax, %eax
	movl $(tabPGDIR - KERNEL_OFFSET), %edi
	movl $0x2000, %ecx
	rep stosb

	// Map first page table to $0x00000000 and $KERNEL_OFFSET.
	movl $(tabPGTAB - KERNEL_OFFSET + 3), (tabPGDIR - KERNEL_OFFSET)
	movl $(tabPGTAB - KERNEL_OFFSET + 3), (tabPGDIR - KERNEL_OFFSET + (KERNEL_OFFSET >> 20))

	// Recursive mapping of page table.
	movl $(tabPGDIR - KERNEL_OFFSET + 3), (tabPGDIR - KERNEL_OFFSET + 0x0ffc)

	// Map IVT and BDA read only.
	mappages 0x00000000, 0x00001000, 0x001
	mappages 0x0009f000, 0x000a0000, 0x001

	// Map AP startup trampoline.
	mappages 0x00003000, 0x00004000, 0x003

	// Map VGA buffer read write (32 entries * 4kB = 128kB).
	mappages 0x000a0000, 0x000c0000, 0x003

	// Map BIOS read only (32 entries * 4kB = 128kB).
	mappages 0x000e0000, 0x00100000, 0x001

	// Map text part of startup section.
	mappages2 (initStart - KERNEL_OFFSET), (initTextEnd - KERNEL_OFFSET), 0x001

	// Map data part of startup section.
	mappages2 (initDataStart - KERNEL_OFFSET), (initDataEnd - KERNEL_OFFSET), 0x003

	// Map text part of kernel.
	mappages2 (textStart - KERNEL_OFFSET), (textEnd - KERNEL_OFFSET), 0x001

	// Map data part of kernel.
	mappages2 (dataStart - KERNEL_OFFSET), (kernelEnd - KERNEL_OFFSET), 0x003

	// Enter page directory in CR3.
	movl $(tabPGDIR - KERNEL_OFFSET), %eax
	movl %eax, %cr3

	// Enable paging and floating point exception.
	movl %cr0, %ecx
	orl $0x8000002a, %ecx
	movl %ecx, %cr0

	// Clear the IDT and GDT.
	xorl %eax, %eax
	movl $tabIDT, %edi
	movl $(IDT_LENGTH + GDT_LENGTH), %ecx
	rep stosb

	// Restore EAX.
	movl %ebp, %eax

	// Setup a bare bones GDT.
	movl $myGDT, %esi
	movl $tabGDT, %edi
	movl $(8 * 8), %ecx
	rep movsb

	lgdt pGDT
	lidt pIDT
	lldt pLDT

	movw $KERNEL_DATA, %dx
	movw %dx, %ds
	movw %dx, %es
	movw %dx, %fs
	movw %dx, %gs
	movw $KERNEL_STACK, %dx
	movw %dx, %ss

	// Load CS with new selector.
	ljmp $KERNEL_CODE, $FlushGDT
.size Entry, . - Entry

.section .text
.type FlushGDT, @function
FlushGDT:
	// Clear identity mapping, preserve only kernel area.
	movl $0, (0xfffff000)

	// Set up kernel stack.
	movl $bspStack, %esp

	// Push the multiboot info structure and magic.
	pushl %ebx
	pushl %eax

	// Everything is set up for C / C++, so we can call the main entry function.
	call KernelEntry
	addl $8, %esp

	// Free init memory.
	call FreeMemory
	pushl $0

	// Determine address if this CPU's TSS.
	str %eax
	shrl $3, %eax
	subl $FIRST_TSS, %eax
	movl $TSS_LENGTH, %ebx
	mull %ebx
	addl $TSS_LIN_ADDR, %eax

	// Save kernel SS:ESP in TSS.
	movl $KERNEL_STACK, 8(%eax)
	movl %esp, 4(%eax)

	// Enable interrupts -> enable multitasking.
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

	// Read and increment AP count.
	movl $1, %eax
	movl $(apcount), %edx
	lock xaddl %eax, (%edx)
	incl %eax
	movl %eax, %ebx
	incl %eax

	// Set stack address.
	movl $STACK_LIN_ADDR, %esp
	movl $STACK_SIZE, %edx
	mull %edx
	addl %eax, %esp

	// Set TSS.
	addl $FIRST_TSS, %ebx
	shll $3, %ebx
	ltr %bx

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
.size ap_in_pmode, . - ap_in_pmode

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

.section .init.text
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
apcount: .long 0
.size apcount, . - apcount
