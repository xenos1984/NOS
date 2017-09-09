.section .init.text

.globl Entry
.extern KernelEntry

Entry:
// Set stack pointer to below kernel.
	mov sp, #0x8000

// Clear BSS and page tables
	ldr r4, =bssStart
	ldr r9, =kernelEnd
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
	ldr r5, =tabPGTAB
	orr r5, r5, #1
	ldr r4, =tabPGDIR
	str r5, [r4]
	add r4, r4, #0x3000
	str r5, [r4]

// Fill page table to map kernel
	ldr r9, =kernelEnd
	ldr r5, =initTextStart
	ldr r4, =tabPGTAB
	mov r6, r5, LSR#10
	add r4, r4, r6
	ldr r7, =#0xff2
	orr r5, r5, r7
	b 4f

3:
	str r5, [r4], #4
	add r5, #0x1000

4:
	cmp r5, r9
	blo 3b

// Enable MMU
	ldr r4, =tabPGDIR
	mcr p15, 0, r4, c2, c0
	mov r4, #5
	mcr p15, 0, r4, c3, c0
	ldr r4, =0x101
	mcr p15, 0, r4, c1, c0
	nop
	nop
	nop

// Call kernel entry point.
	ldr r3, =KernelEntry
	ldr r4, =kernelOffset
	add r3, r3, r4
	blx r3

// Infinite loop.
5:
	wfe
	b 5b

