.section .init.text

#define KERNEL_OFFSET 0xC0000000

.globl Entry
.extern KernelEntry

Entry:
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
	str r5, [r4]
	add r4, r4, #0x3000
	str r5, [r4]

// Fill page table to map kernel
	ldr r9, =kernelEnd - KERNEL_OFFSET
	ldr r5, =kernelStart - KERNEL_OFFSET
	ldr r4, =tabPGTAB - KERNEL_OFFSET
	mov r6, r5, LSR#10
	add r4, r4, r6
	ldr r7, =#0x452
	orr r5, r5, r7
	b 4f

3:
	str r5, [r4], #4
	add r5, #0x1000

4:
	cmp r5, r9
	blo 3b

// Enable MMU
	ldr r4, =tabPGDIR - KERNEL_OFFSET
	mcr p15, 0, r4, c2, c0
	mov r4, #5
	mcr p15, 0, r4, c3, c0
	ldr r4, =0x101
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

