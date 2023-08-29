// InterruptStub.S - Assembler part of interrupt routines

#include <arch/x86/x64/Entry.h>

#define PUSHALL \
	pushq %rax; \
	pushq %rcx; \
	pushq %rdx; \
	pushq %rbx; \
	pushq %rsp; \
	addq $32, 0(%rsp); \
	pushq %rbp; \
	pushq %rsi; \
	pushq %rdi; \
	pushq %r8; \
	pushq %r9; \
	pushq %r10; \
	pushq %r11; \
	pushq %r12; \
	pushq %r13; \
	pushq %r14; \
	pushq %r15;

#define POPALL \
	popq %r15; \
	popq %r14; \
	popq %r13; \
	popq %r12; \
	popq %r11; \
	popq %r10; \
	popq %r9; \
	popq %r8; \
	popq %rdi; \
	popq %rsi; \
	popq %rbp; \
	addq $8, %rsp; \
	popq %rbx; \
	popq %rdx; \
	popq %rcx; \
	popq %rax;

#define INTERRUPT(name) \
.global name##Wrapper; \
.type name##Wrapper, @function; \
.align 16; \
.extern User##name; \
.extern Kernel##name; \
name##Wrapper: \
	cli; \
	testq $3, 8(%rsp); \
	jz 1f; \
	swapgs; \
	movq %rax, %gs:8; \
	movq %rbx, %gs:16; \
	movq %rcx, %gs:24; \
	movq %rdx, %gs:32; \
	movq %rsi, %gs:40; \
	movq %rdi, %gs:48; \
	movq %rbp, %gs:56; \
	movq %r8, %gs:72; \
	movq %r9, %gs:80; \
	movq %r10, %gs:88; \
	movq %r11, %gs:96; \
	movq %r12, %gs:104; \
	movq %r13, %gs:112; \
	movq %r14, %gs:120; \
	movq %r15, %gs:128; \
	movq 24(%rsp), %rax; \
	movq %rax, %gs:64; \
	movq 16(%rsp), %rax; \
	movq %rax, %gs:136; \
	movq 0(%rsp), %rax; \
	movq %rax, %gs:0; \
	swapgs; \
	addq $40, %rsp; \
	movabsq $(User##name), %rax; \
	call *%rax; \
	jmp ReturnToUser; \
1: \
	PUSHALL \
	movq %rsp, %rdi; \
	movabsq $(Kernel##name), %rax; \
	call *%rax; \
	POPALL \
	hlt; \
.size name##Wrapper, . - name##Wrapper

#define EXCEPTION(name) \
.global name##Wrapper; \
.type name##Wrapper, @function; \
.align 16; \
.extern User##name; \
.extern Kernel##name; \
name##Wrapper: \
	cli; \
	testq $3, 16(%rsp); \
	jz 1f; \
	swapgs; \
	movq %rax, %gs:8; \
	movq %rbx, %gs:16; \
	movq %rcx, %gs:24; \
	movq %rdx, %gs:32; \
	movq %rsi, %gs:40; \
	movq %rdi, %gs:48; \
	movq %rbp, %gs:56; \
	movq %r8, %gs:72; \
	movq %r9, %gs:80; \
	movq %r10, %gs:88; \
	movq %r11, %gs:96; \
	movq %r12, %gs:104; \
	movq %r13, %gs:112; \
	movq %r14, %gs:120; \
	movq %r15, %gs:128; \
	movq 32(%rsp), %rax; \
	movq %rax, %gs:64; \
	movq 24(%rsp), %rax; \
	movq %rax, %gs:136; \
	movq 8(%rsp), %rax; \
	movq %rax, %gs:0; \
	swapgs; \
	popq %rdi; \
	addq $40, %rsp; \
	movabsq $(User##name), %rax; \
	call *%rax; \
	jmp ReturnToUser; \
1: \
	xchgq %rax, 0(%rsp); \
	pushq %rcx; \
	pushq %rdx; \
	pushq %rbx; \
	pushq %rsp; \
	addq $32, 0(%rsp); \
	pushq %rbp; \
	pushq %rsi; \
	pushq %rdi; \
	pushq %r8; \
	pushq %r9; \
	pushq %r10; \
	pushq %r11; \
	pushq %r12; \
	pushq %r13; \
	pushq %r14; \
	pushq %r15; \
	movq %rsp, %rdi; \
	movq %rax, %rsi; \
	movabsq $(Kernel##name), %rax; \
	call *%rax; \
	POPALL \
	hlt; \
.size name##Wrapper, . - name##Wrapper

.extern HardwareHandler

#define HARDWARE(num) \
.global Hardware##num; \
.type Hardware##num, @function; \
.align 16; \
Hardware##num: \
	testq $3, 8(%rsp); \
	jz 1f; \
	swapgs; \
	movq %rax, %gs:8; \
	movq %rbx, %gs:16; \
	movq %rcx, %gs:24; \
	movq %rdx, %gs:32; \
	movq %rsi, %gs:40; \
	movq %rdi, %gs:48; \
	movq %rbp, %gs:56; \
	movq %r8, %gs:72; \
	movq %r9, %gs:80; \
	movq %r10, %gs:88; \
	movq %r11, %gs:96; \
	movq %r12, %gs:104; \
	movq %r13, %gs:112; \
	movq %r14, %gs:120; \
	movq %r15, %gs:128; \
	movq 24(%rsp), %rax; \
	movq %rax, %gs:64; \
	movq 16(%rsp), %rax; \
	movq %rax, %gs:136; \
	movq 0(%rsp), %rax; \
	movq %rax, %gs:0; \
	swapgs; \
	addq $40, %rsp; \
	movabsq $num, %rdi; \
	movabsq $(HardwareHandler), %rax; \
	call *%rax; \
	jmp ReturnToUser; \
1: \
	addq $40, %rsp; \
	movabsq $num, %rdi; \
	movabsq $(HardwareHandler), %rax; \
	call *%rax; \
	jmp ReturnToUser; \
.size Hardware##num, . - Hardware##num

#define TIMER(name) \
.global name##Wrapper; \
.type name##Wrapper, @function; \
.extern name; \
.align 16; \
name##Wrapper: \
	testq $3, 8(%rsp); \
	jz 1f; \
	swapgs; \
	movq %rax, %gs:8; \
	movq %rbx, %gs:16; \
	movq %rcx, %gs:24; \
	movq %rdx, %gs:32; \
	movq %rsi, %gs:40; \
	movq %rdi, %gs:48; \
	movq %rbp, %gs:56; \
	movq %r8, %gs:72; \
	movq %r9, %gs:80; \
	movq %r10, %gs:88; \
	movq %r11, %gs:96; \
	movq %r12, %gs:104; \
	movq %r13, %gs:112; \
	movq %r14, %gs:120; \
	movq %r15, %gs:128; \
	movq 24(%rsp), %rax; \
	movq %rax, %gs:64; \
	movq 16(%rsp), %rax; \
	movq %rax, %gs:136; \
	movq 0(%rsp), %rax; \
	movq %rax, %gs:0; \
	swapgs; \
	addq $40, %rsp; \
	movabsq $(name), %rax; \
	call *%rax; \
	jmp ReturnToUser; \
1: \
	addq $40, %rsp; \
	movabsq $(name), %rax; \
	call *%rax; \
	jmp ReturnToUser; \
.size name##Wrapper, . - name##Wrapper

.section .text

INTERRUPT(DivideError)
INTERRUPT(DebugInterrupt)
INTERRUPT(NonMaskable)
INTERRUPT(BreakPoint)
INTERRUPT(Overflow)
INTERRUPT(Arraybounds)
INTERRUPT(InvalidOpcode)
INTERRUPT(DeviceNotAvailable)
EXCEPTION(DoubleFault)
INTERRUPT(CoprocSegOverrun)
EXCEPTION(InvalidTSS)
EXCEPTION(SegmentNotPresent)
EXCEPTION(StackFault)
EXCEPTION(GeneralProtection)
EXCEPTION(PageFault)
INTERRUPT(CoprocError)
EXCEPTION(AlignmentCheck)
INTERRUPT(MachineCheck)
INTERRUPT(SSEFault)

TIMER(ApicTimerIRQ)
TIMER(PITIRQ)

.type ReturnToUser, @function
.align 16
ReturnToUser:
	movl $0xc0000102, %ecx
	rdmsr
	shlq $32, %rdx
	movl %eax, %eax
	lea (%rdx,%rax,1), %rax
	testq %rax, %rax
	jz 1f
	swapgs
	pushq $USER_STACK
	pushq %gs:64
	pushq %gs:136
	pushq $USER_CODE
	pushq %gs:0
	movq %gs:128, %r15
	movq %gs:120, %r14
	movq %gs:112, %r13
	movq %gs:104, %r12
	movq %gs:96, %r11
	movq %gs:88, %r10
	movq %gs:80, %r9
	movq %gs:72, %r8
	movq %gs:56, %rbp
	movq %gs:48, %rdi
	movq %gs:40, %rsi
	movq %gs:32, %rdx
	movq %gs:24, %rcx
	movq %gs:16, %rbx
	movq %gs:8, %rax
	swapgs
	iretq
1:
	sti
	hlt
	jmp 1b
.size ReturnToUser, . - ReturnToUser

.global SpuriousIRQWrapper
.type SpuriousIRQWrapper, @function
SpuriousIRQWrapper:
	iretq
.size SpuriousIRQWrapper, . - SpuriousIRQWrapper

.global sysentry
.type sysentry, @function
.type jumpMin, @object
.type jumpMax, @object
.extern userMax
.extern kernelElfDynsym
.align 16
jumpMin:
	.quad USER_OFFSET
.size jumpMin, . - jumpMin
jumpMax:
	.quad userMax
.size jumpMax, . - jumpMax

sysentry:
	cli
	pushq %rax
	movq 8(%rsp), %rax
	cmpq %rax, jumpMin(%rip) // below trampoline?
	jae 1f
	cmpq %rax, jumpMax(%rip) // above trampoline?
	jb 1f
	popq %rax
	swapgs
	movq %rax, %gs:8
	movq %rbx, %gs:16
	movq %rcx, %gs:24
	movq %rdx, %gs:32
	movq %rsi, %gs:40
	movq %rdi, %gs:48
	movq %rbp, %gs:56
	movq %r8, %gs:72
	movq %r9, %gs:80
	movq %r10, %gs:88
	movq %r11, %gs:96
	movq %r12, %gs:104
	movq %r13, %gs:112
	movq %r14, %gs:120
	movq %r15, %gs:128
	movq 24(%rsp), %rax
	movq %rax, %gs:64
	movq 16(%rsp), %rax
	movq %rax, %gs:136
	movq 0(%rsp), %rax
	movq %rax, %gs:0
	swapgs
	movabsq $USER_OFFSET, %rbx
	subq %rbx, %rax
	shrq $4, %rax
	lea (%rax,%rax,2), %rax
	movabsq $(kernelElfDynsym), %rbx
	movq 8(%rbx,%rax,8), %rax
	addq $40, %rsp
	call *%rax
	jmp ReturnToUser

1:
	popq %rax
	iretq
.size sysentry, . - sysentry

.section .user
.rept 256
	.align 16
	int $0xf0
	ret
.endr
