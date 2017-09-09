.section .init.text

.globl Entry
.extern KernelEntry

Entry:
	mov sp, #0x8000
	ldr r4, =bssStart
	ldr r9, =bssEnd
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

	ldr r3, =KernelEntry
	blx r3

3:
	wfe
	b 3b

