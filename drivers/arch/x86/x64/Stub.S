// Stub.S - Driver stub for x86_64 architecture.

.global _start
.extern Constructor

#define STACK_SIZE 0x1000

.code64
.section .text
_start:
	movabsq $(stack + STACK_SIZE), %rsp
	movabsq (Constructor), %rax
	call *%rax
	jmp .

.section .bss
	.align 4
	.comm stack, STACK_SIZE
	.space STACK_SIZE
