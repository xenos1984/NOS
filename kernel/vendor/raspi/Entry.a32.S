// Entry.S - Kernel entry point

#include <vendor/raspi/Entry.h>
#include <arch/arm/Extensions.h>

.section .init.text

.globl Entry
.extern KernelEntry

Entry:
#ifdef ARM_EXT_SECURITY
// Check whether we are in secure state (QEMU) and drop.
	mrc p15, 0, r4, c1, c1, 0
	ands r4, r4, #1
	bne EntryNS
	mrc p15, 0, r4, c1, c1, 2
	orr r0, #0xc00
	mcr p15, 0, r4, c1, c1, 2
	ldr r4, =MonitorExceptionVectorBase - KERNEL_OFFSET
	mcr p15, 0, r4, c12, c0, 1
	mrc p15, 0, r4, c1, c1, 0
	orr r4, #0x31
	mcr p15, 0, r4, c1, c1, 0
	smc #0

EntryNS:
#endif

#ifdef ARCH_ARM_V7
// Check for CPU ID - let CPU 0 set up the basic structures, before others join.
	mrc p15, 0, r4, c0, c0, 5
	ands r4, r4, #3
	beq EntryBSP

// This is an application processor (CPU > 0), put into idle loop for now.
EntryAP:
	wfe
	b EntryAP

EntryBSP:
#endif
// Set stack pointer to kernel stack.
	ldr sp, =bspStack

// Clear BSS and page tables
	ldr r4, =bssStart - KERNEL_OFFSET
	ldr r9, =kernelEnd - KERNEL_OFFSET
	mov r5, #0
	mov r6, #0
	mov r7, #0
	mov r8, #0
	b 2f

1:
	stmia r4!, {r5-r8}

2:
	cmp r4, r9
	blo 1b

// Link page table to identity mapping and higher half
	ldr r5, =tabPGTAB - KERNEL_OFFSET
	orr r5, r5, #1
	ldr r4, =tabPGDIR - KERNEL_OFFSET
	add r6, r4, #(KERNEL_OFFSET >> 18)
	add r9, r4, #16
	b 4f

3:
	str r5, [r4], #4
	str r5, [r6], #4
	add r5, #0x400

4:
	cmp r4, r9
	blo 3b

// Fill page table to map kernel
	ldr r9, =kernelEnd - KERNEL_OFFSET
	ldr r5, =kernelStart - KERNEL_OFFSET
	ldr r4, =tabPGTAB - KERNEL_OFFSET
	mov r6, r5, LSR#10
	add r4, r4, r6
	ldr r7, =#0x452
	orr r5, r5, r7
	b 6f

5:
	str r5, [r4], #4
	add r5, #0x1000

6:
	cmp r5, r9
	blo 5b

// Map page table into paging structures
	ldr r4, =tabPGTAB - KERNEL_OFFSET + (KERNEL_OFFSET >> 20)
	ldr r5, =tabPGTAB - KERNEL_OFFSET
	ldr r7, =#0x452
	orr r5, r5, r7
	str r5, [r4]

// Map peripherals into virtual memory
	ldr r4, =tabPGDIR - KERNEL_OFFSET + (PERI_OFFSET >> 18)
	add r9, r4, #64
	ldr r5, =#0x50412 + PERI_PHYS
	b 8f

7:
	str r5, [r4], #4

8:
	cmp r4, r9
	blo 7b

// Set TTBR0 and TTBR1
	ldr r4, =tabPGDIR - KERNEL_OFFSET
	mcr p15, 0, r4, c2, c0, 0
	mcr p15, 0, r4, c2, c0, 1
// Configure how many bits to share
	mov r4, #TABLE_SPLIT_BITS
	mcr p15, 0, r4, c2, c0, 2
// Set Domain Access Control Register
	mov r4, #5
	mcr p15, 0, r4, c3, c0
// Enable MMU, disable subpages
	mrc p15, 0, r4, c1, c0
	ldr r7, =#0x800001
	orr r4, r4, r7
	mcr p15, 0, r4, c1, c0
	nop
	nop
	nop

// Jump to higher half
	ldr pc, =higherhalf

higherhalf:

// Call kernel entry point.
	ldr r3, =KernelEntry
	blx r3

// Infinite loop.
1:
	wfe
	b 1b

#ifdef ARM_EXT_SECURITY
.align 5
MonitorExceptionVectorBase:
b . // Unused
b . // Unused
movs pc, lr // Secure Monitor Call
b . // Prefetch abort
b . // Data abort
b . // Unused
b . // IRQ
b . // FIQ
#endif
