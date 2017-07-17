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

#include <arch/x86/amd64/Entry.h>

.macro mappages from to flags
	movl $((\to - \from) >> 12), %ecx
	movl $(\from | \flags), %eax
	movl $(tabPGTAB - KERNEL_OFFSET + (\from >> 9)), %edi
1:
	stosl
	addl $0x1000, %eax
	movl $0, (%edi)
	addl $4, %edi
	loop 1b
.endm

.macro mappages2 from to flags
	movl $\to, %ecx
	subl $\from, %ecx
	shrl $12, %ecx
	movl $\from, %eax
	movl %eax, %edi
	orl $\flags, %eax
	shrl $9, %edi
	addl $(tabPGTAB - KERNEL_OFFSET), %edi
1:
	stosl
	addl $0x1000, %eax
	movl $0, (%edi)
	addl $4, %edi
	loop 1b
.endm

.section .init.text
.code32
.type no_long_mode, @function
no_long_mode:
	// print error message
	mov $(cpu_error - KERNEL_OFFSET), %esi
	mov $0xb8000, %edi
1:
	movsb
	movb $12, (%edi)
	incl %edi
	cmpb $0, (%esi)
	jne 1b

2:
	// flush keyboard buffer
	inb $0x64, %al
	test $0x01, %al
	jz 3f
	inb $0x60, %al
	jmp 2b

3:
	// wait for key pressed
	inb $0x64, %al
	test $0x01, %al
	jz 3b

	// reboot by causing a triple fault (there are no interrupts so far)
	int $0xff
.size no_long_mode, . - no_long_mode

.global Entry
.type Entry, @function
Entry:
	// make sure we're going in the right direction!
	cld
	cli

	// clear the idt + gdt + tss @ IDT_PHYS_ADDR
	pushl %eax
	xorl %eax, %eax
	movl $(tabIDT - KERNEL_OFFSET), %edi
	movl $(IDT_LENGTH + GDT_LENGTH), %ecx
	rep stosb
	popl %eax

	// setup a bare bones GDT
	movl $(myGDT - KERNEL_OFFSET), %esi
	movl $(tabGDT - KERNEL_OFFSET), %edi
	movl $(10 * 8), %ecx
	rep movsb

	pushl %eax
	pushl %ebx

	// check if long mode is supported
	movl $0x80000000, %eax // Extended-function 80000000h.
	cpuid // Is largest extended function
	cmpl $0x80000000, %eax // any function > 80000000h?
	jbe no_long_mode // If not, no long mode.
	movl $0x80000001, %eax // Extended-function 80000001h.
	cpuid // Now EDX = extended-features flags.
	btl $29, %edx // Test if long mode is supported.
	jnc no_long_mode // Exit if not supported.

	popl %ebx

	// create page tables, first clear them
	xorl %eax, %eax
	movl $(tabPML4T - KERNEL_OFFSET), %edi
	movl $0x4000, %ecx
	rep stosb

	// chain tables together
	movl $(tabPGDIP - KERNEL_OFFSET + 3), (tabPML4T - KERNEL_OFFSET)
	movl $(tabPGDIP - KERNEL_OFFSET + 3), (tabPML4T - KERNEL_OFFSET + 0xff8)
	movl $(tabPGDIR - KERNEL_OFFSET + 3), (tabPGDIP - KERNEL_OFFSET)
	movl $(tabPGDIR - KERNEL_OFFSET + 3), (tabPGDIP - KERNEL_OFFSET + 0xff0)
	movl $(tabPGTAB - KERNEL_OFFSET + 3), (tabPGDIR - KERNEL_OFFSET)

	// enter PML4 as second but last PDP, causing a chain reaction to mirror the tables into virtual memory
	movl $(tabPML4T - KERNEL_OFFSET + 3), (tabPML4T - KERNEL_OFFSET + 0xff0)

	// Map IVT and BDA read only.
	mappages 0x00000000, 0x00001000, 0x001
	mappages 0x0009f000, 0x000a0000, 0x001

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

	// enable pae + pse + pge
	movl %cr4, %eax
	orl $0xb0, %eax
	movl %eax, %cr4

	// set page dir base
	movl $(tabPML4T - KERNEL_OFFSET), %eax
	movl %eax, %cr3

	// enable long mode and SYSCALL / SYSRET
	movl $0xc0000080, %ecx
	rdmsr
	orl $0x00000101, %eax
	wrmsr

	popl %eax

	// enable paging (activate long mode), enable floating point exception
	movl %cr0, %ecx
	orl $0x8000002a, %ecx
	movl %ecx, %cr0

	lgdt (pGDT32 - KERNEL_OFFSET)

	// jump into long mode
	ljmpl $0x20, $(LongMode - KERNEL_OFFSET)
.size Entry, . - Entry

.section .text
.code64
.type LongMode, @function
LongMode:
	movabsq $bspStack, %rsp
	movabsq $1f, %rdx
	jmpq *%rdx

1:
	// put the multiboot info and magic into the right registers so our C code will find them
	xorq %rdi, %rdi
	xorq %rsi, %rsi

	movl %eax, %edi
	movl %ebx, %esi

	movabsq $(set_cpu_regs), %rax
	callq *%rax

	// clear identity mapping, preserve only kernel area
	movabsq $tabPML4T, %rax
	movl $0x0, 0(%rax)
	movabsq $tabPGDIP, %rax
	movl $0x0, 0(%rax)

	// time for some C!
	movabsq $(KernelEntry), %rax
	callq *%rax // Run the boot module by calling its C++ entry point
	movabsq $(FreeMemory), %rax
	callq *%rax
	str %rax
	shrq $4, %rax
	subq $FIRST_TSS, %rax
	movabsq $TSS_LENGTH, %rbx
	mulq %rbx
	movabsq $TSS_LIN_ADDR, %rbx
	addq %rbx, %rax
	movq %rsp, 4(%rax)

2:
	sti
	hlt
	jmp 2b
.size LongMode, . - LongMode

.type set_cpu_regs, @function
set_cpu_regs:
	// reload descriptor tables
	movabsq $(pIDT64), %rax

	lidt 0(%rax)  // load the IDT
	lgdt 10(%rax) // load the GDT
	lldt 20(%rax) // load the LDT (0)

	// set 64bit FS and GS offset to 0
	xorl %eax, %eax
	movl %eax, %edx
	movl $0xc0000100, %ecx
	wrmsr
	incq %rcx
	wrmsr

	movw %ax, %ds
	movw %ax, %es
	movw %ax, %fs
	movw %ax, %gs
	movw %ax, %ss

	ret
.size set_cpu_regs, . - set_cpu_regs

#if defined CONFIG_SMP || defined CONFIG_ACPI
.type ap_in_long_mode, @function
ap_in_long_mode:
	movabsq $1f, %rdx
	jmpq *%rdx

1:
	movabsq $(apstack), %rax
	movq (%rax), %rsp
	movabsq $(set_cpu_regs), %rax
	callq *%rax

	// time for some C!
	movabsq $(ApplicationEntry), %rax
	callq *%rax
	str %rax
	shrq $4, %rax
	subq $FIRST_TSS, %rax
	movabsq $TSS_LENGTH, %rbx
	mulq %rbx
	movabsq $TSS_LIN_ADDR, %rbx
	addq %rbx, %rax
	movq %rsp, 4(%rax)

2:
	sti
	hlt
	jmp 2b
.size ap_in_long_mode, . - ap_in_long_mode

.section .init.text
.code16
.global apentry
.type apentry, @function
apentry:
	cli
	movw %cs, %ax
	movw %ax, %ds

	// enable pae + pse + pge
	movl %cr4, %eax
	orl $0xb0, %eax
	movl %eax, %cr4

	// set page dir base
	movl $(tabPML4T - KERNEL_OFFSET), %eax
	movl %eax, %cr3

	// enable long mode
	movl $0xc0000080, %ecx
	rdmsr
	btsl $8, %eax
	wrmsr

	movl %cr0, %eax
	orl $0x8000002b, %eax // enable pmode + paging to get to long mode, enable floating point exception
	movl %eax, %cr0

	lgdt (pGDT32 - apentry)

	ljmpl $0x20, $(ap_in_long_mode - KERNEL_OFFSET)
.size apentry, . - apentry

#endif

// need them here since APs can't access .data from real mode

.type pGDT32, @object
pGDT32 : .word GDT_LENGTH
         .long tabGDT - KERNEL_OFFSET
.size pGDT32, . - pGDT32

.type pIDT64, @object
pIDT64 : .word IDT_LENGTH // limit of 256 IDT slots
         .quad tabIDT     // starting at tabIDT
.size pIDT64, . - pIDT64

.type pGDT64, @object
pGDT64 : .word GDT_LENGTH // 256 GDT slots
         .quad tabGDT     // starting at tabGDT (after IDT)
.size pGDT64, . - pGDT64

.type pLDT64, @object
pLDT64 : .word 0          // no LDT - load with 0
         .quad 0
.size pLDT64, . - pLDT64

.section .init.data

.align 8

.type myGDT, @object
myGDT:
	// 0x00 = Null descriptor
	.long 0
	.long 0

	// 0x08 = 32bit data segment
	.long 0x0000ffff
	.long 0x00cf9200

	// 0x10 = 32bit code segment
	.long 0x0000ffff
	.long 0x00cf9A00

	// 0x18 = 32bit stack segment
	.long 0x0000ffff
	.long 0x00cf9200

	// 0x20 = kernel code segment
	.long 0x0000ffff
	.long 0x002f9a00

	// 0x28 = kernel stack segment
	.long 0x0000ffff
	.long 0x00cf9200

	// 0x30 = kernel data segment
	.long 0x0000ffff
	.long 0x00cf9200

	// 0x38 = user data segment
	.long 0x0000ffff
	.long 0x00cff200

	// 0x40 = user stack segment
	.long 0x0000ffff
	.long 0x00cff200

	// 0x48 = user code segment
	.long 0x0000ffff
	.long 0x002ffa00
.size myGDT, . - myGDT

.global apstack
.type apstack, @object
apstack: .quad 0
.size apstack, . - apstack

.type cpu_error, @object
cpu_error:
	.asciz "ERROR: This CPU does not support long mode. Press any key to reboot."
.size cpu_error, . - cpu_error
