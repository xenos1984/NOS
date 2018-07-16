// Entry.S - Kernel entry point

#include <vendor/raspi/Entry.h>

.section .init.text

.globl Entry
.extern KernelEntry

Entry:
// Check for CPU ID - let CPU 0 set up the basic structures, before others join.
	mrs x4, mpidr_el1
	and x4, x4, #3
	cbz x4, EntryBSP

// This is an application processor (CPU > 0), put into idle loop for now.
EntryAP:
	wfe
	b EntryAP

EntryBSP:
// Set stack pointer to kernel stack.
	ldr x4, =bspStack
	mov sp, x4

// Clear BSS and page tables
	ldr x4, =bssStart - KERNEL_OFFSET
	ldr x9, =kernelEnd - KERNEL_OFFSET
	b 2f

1:
	str xzr, [x4], #8

2:
	cmp x4, x9
	blo 1b

// Link page table to identity mapping and higher half
	mov x7, #0x703
	ldr x5, =tabPGL2 - KERNEL_OFFSET
	orr x5, x5, x7
	ldr x4, =tabPGL1 - KERNEL_OFFSET
	str x5, [x4]
	add x4, x4, #0xff8
	str x5, [x4]

	ldr x5, =tabPGL3 - KERNEL_OFFSET
	orr x5, x5, x7
	ldr x4, =tabPGL2 - KERNEL_OFFSET
	str x5, [x4]
	add x4, x4, #0xff8
	str x5, [x4]

// Fill page table to map kernel
	ldr x9, =kernelEnd - KERNEL_OFFSET
	ldr x5, =kernelStart - KERNEL_OFFSET
	ldr x4, =tabPGL3 - KERNEL_OFFSET
	mov x6, x5, LSR#9
	add x4, x4, x6
	orr x5, x5, x7
	b 4f

3:
	str x5, [x4], #8
	add x5, x5, #0x1, LSL #12

4:
	cmp x5, x9
	blo 3b

// Set memory attributes array
	ldr x4, =0x4404ff
	msr mair_el1, x4
// Set translate control register
	mrs x4, id_aa64mmfr0_el1
	and x4, x4, #0xf
	lsl x4, x4, #32
	ldr x6, =0xb5193519
	orr x4, x4, x6
	msr tcr_el1, x4
	isb
// Set TTBR0 and TTBR1
	ldr x4, =tabPGL1 - KERNEL_OFFSET
	msr ttbr0_el1, x4
	msr ttbr1_el1, x4
// Enable paging
	dsb ish
	isb
	mrs x4, sctlr_el1
	ldr x6, =0x00c00801
	orr x4, x4, x6
	ldr x6, =0xfcf7efe1
	and x4, x4, x6
	msr sctlr_el1, x4
	isb

// Jump to higher half
	b higherhalf

higherhalf:

// Call kernel entry point.
	bl KernelEntry

// Infinite loop.
1:
	wfe
	b 1b

