// Stub.i386.S - Driver stub for i386 architecture.

.global _start
.extern Constructor

#define STACK_SIZE 0x1000

.code32
.section .text
_start:
	movl $(stack + STACK_SIZE), %esp
	movl (Constructor), %eax
	pushl $0
	call *%eax
	jmp .

.section .bss
	.align 4
	.comm stack, STACK_SIZE
	.space STACK_SIZE
