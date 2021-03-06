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
// Check EL
	mrs x0, CurrentEL
	and x0, x0, #12

// EL3?
	cmp x0, #12
	bne EL2

// Yes, EL3.
	mov x2, #0x5b1
	msr scr_el3, x2
	mov x2, #0x3c9
	msr spsr_el3, x2
	adr x2, EL2
	msr elr_el3, x2
	eret

EL2:
// EL2?
	cmp x0, #4
	beq EL1
	msr sp_el1, x1
// Enable CNTP for EL1.
	mrs x0, cnthctl_el2
	orr x0, x0, #3
	msr cnthctl_el2, x0
	msr cntvoff_el2, xzr
// Enable AArch64 in EL1.
	mov x0, #(1 << 31) // AArch64
	orr x0, x0, #(1 << 1) // SWIO hardwired on Pi3
	msr hcr_el2, x0
	mrs x0, hcr_el2
// Setup SCTLR access.
	mov x2, #0x0800
	movk x2, #0x30d0, lsl #16
	msr sctlr_el1, x2
// Change execution level to EL1h
	mov x2, #0x3c5
	msr spsr_el2, x2
	adr x2, EL1
	msr elr_el2, x2
	eret

EL1:
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

// Link page table to recursive mapping, identity mapping and higher half
	mov x7, #0x703
	ldr x5, =tabPGL2 - KERNEL_OFFSET
	orr x5, x5, x7
	ldr x4, =tabPGL1 - KERNEL_OFFSET
	orr x6, x4, x7
	str x5, [x4]
	add x4, x4, #0xff0
	str x6, [x4]
	add x4, x4, #0x8
	str x5, [x4]

	ldr x5, =tabPGL3 - KERNEL_OFFSET
	orr x5, x5, x7
	ldr x4, =tabPGL2 - KERNEL_OFFSET
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

// Map peripherals into virtual memory
	ldr x4, =tabPGL2 + 0x3c0 - KERNEL_OFFSET
	add x9, x4, #64
	ldr x5, =0x3f000605
	b 6f

5:
	str x5, [x4], #8
	add x5, x5, #0x200, LSL #12

6:
	cmp x4, x9
	blo 5b

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
	ldr x4, =tabPGL1 - KERNEL_OFFSET + 1
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
	nop
	nop
	nop

// Jump to higher half
	ldr x3, =higherhalf
	br x3

higherhalf:

// Disable lower half / identity mapping, force use of higher half
	ldr x4, =tabPGL1 - KERNEL_OFFSET
	str xzr, [x4]
	mrs x4, tcr_el1
	and x4, x4, #0xffffffffffffff7f
	msr tcr_el1, x4
	msr ttbr0_el1, xzr

// Call kernel entry point.
	bl KernelEntry

// Infinite loop.
1:
	wfe
	b 1b

