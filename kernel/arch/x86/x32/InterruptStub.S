// InterruptStub.S - Assembler part of interrupt routines

#include <arch/x86/x32/Entry.h>

#define INTERRUPT(name) \
.global name##Wrapper; \
.type name##Wrapper, @function; \
.extern User##name; \
.extern Kernel##name; \
.align 16; \
name##Wrapper: \
	cli; \
	testl $3, 4(%esp); \
	jz 1f; \
	pushl %eax; \
	movl $KERNEL_DATA, %eax; \
	movl %eax, %ds; \
	movl %eax, %es; \
	movl %eax, %fs; \
	movl %eax, %gs; \
	movl 24(%esp), %eax; \
	movl %ebx, 8(%eax); \
	popl %ebx; \
	movl %ebx, 4(%eax); \
	movl %ecx, 12(%eax); \
	movl %edx, 16(%eax); \
	movl %esi, 20(%eax); \
	movl %edi, 24(%eax); \
	movl %ebp, 28(%eax); \
	movl 12(%esp), %ebx; \
	movl %ebx, 32(%eax); \
	movl 8(%esp), %ebx; \
	movl %ebx, 36(%eax); \
	movl 0(%esp), %ebx; \
	movl %ebx, 0(%eax); \
	addl $20, %esp; \
	call User##name; \
	jmp ReturnToUser; \
1: \
	pushal; \
	pushl %esp; \
	call Kernel##name; \
	addl $4, %esp; \
	popal; \
	hlt; \
.size name##Wrapper, . - name##Wrapper

#define EXCEPTION(name) \
.global name##Wrapper; \
.type name##Wrapper, @function; \
.extern name; \
.align 16; \
name##Wrapper: \
	cli; \
	testl $3, 8(%esp); \
	jz 1f; \
	pushl %eax; \
	movl $KERNEL_DATA, %eax; \
	movl %eax, %ds; \
	movl %eax, %es; \
	movl %eax, %fs; \
	movl %eax, %gs; \
	movl 28(%esp), %eax; \
	movl %ebx, 8(%eax); \
	popl %ebx; \
	movl %ebx, 4(%eax); \
	movl %ecx, 12(%eax); \
	movl %edx, 16(%eax); \
	movl %esi, 20(%eax); \
	movl %edi, 24(%eax); \
	movl %ebp, 28(%eax); \
	movl 16(%esp), %ebx; \
	movl %ebx, 32(%eax); \
	movl 12(%esp), %ebx; \
	movl %ebx, 36(%eax); \
	movl 4(%esp), %ebx; \
	movl %ebx, 0(%eax); \
	movl 0(%esp), %eax; \
	addl $20, %esp; \
	movl %eax, 0(%esp); \
	call User##name; \
	addl $4, %esp; \
	jmp ReturnToUser; \
1: \
	xchgl %eax, 0(%esp); \
	pushl %ecx; \
	pushl %edx; \
	pushl %ebx; \
	pushl %esp; \
	addl $16, 0(%esp); \
	pushl %ebp; \
	pushl %esi; \
	pushl %edi; \
	pushl %eax; \
	pushl %esp; \
	addl $4, 0(%esp); \
	call Kernel##name; \
	addl $8, %esp; \
	popal; \
	hlt; \
.size name##Wrapper, . - name##Wrapper

.extern HardwareHandler

#define HARDWARE(num) \
.global Hardware##num; \
.type Hardware##num, @function; \
.align 16; \
Hardware##num: \
	testl $3, 4(%esp); \
	jz 1f; \
	pushl %eax; \
	movl $KERNEL_DATA, %eax; \
	movl %eax, %ds; \
	movl %eax, %es; \
	movl %eax, %fs; \
	movl %eax, %gs; \
	movl 24(%esp), %eax; \
	movl %ebx, 8(%eax); \
	popl %ebx; \
	movl %ebx, 4(%eax); \
	movl %ecx, 12(%eax); \
	movl %edx, 16(%eax); \
	movl %esi, 20(%eax); \
	movl %edi, 24(%eax); \
	movl %ebp, 28(%eax); \
	movl 12(%esp), %ebx; \
	movl %ebx, 32(%eax); \
	movl 8(%esp), %ebx; \
	movl %ebx, 36(%eax); \
	movl 0(%esp), %ebx; \
	movl %ebx, 0(%eax); \
	addl $20, %esp; \
	pushl $num; \
	call HardwareHandler; \
	addl $4, %esp; \
	jmp ReturnToUser; \
1: \
	addl $12, %esp; \
	pushl $num; \
	call HardwareHandler; \
	addl $4, %esp; \
	jmp ReturnToUser; \
.size Hardware##num, . - Hardware##num

#define TIMER(name) \
.global name##Wrapper; \
.type name##Wrapper, @function; \
.extern name; \
.align 16; \
name##Wrapper: \
	testl $3, 4(%esp); \
	jz 1f; \
	pushl %eax; \
	movl $KERNEL_DATA, %eax; \
	movl %eax, %ds; \
	movl %eax, %es; \
	movl %eax, %fs; \
	movl %eax, %gs; \
	movl 24(%esp), %eax; \
	movl %ebx, 8(%eax); \
	popl %ebx; \
	movl %ebx, 4(%eax); \
	movl %ecx, 12(%eax); \
	movl %edx, 16(%eax); \
	movl %esi, 20(%eax); \
	movl %edi, 24(%eax); \
	movl %ebp, 28(%eax); \
	movl 12(%esp), %ebx; \
	movl %ebx, 32(%eax); \
	movl 8(%esp), %ebx; \
	movl %ebx, 36(%eax); \
	movl 0(%esp), %ebx; \
	movl %ebx, 0(%eax); \
	addl $20, %esp; \
	call name; \
	jmp ReturnToUser; \
1: \
	addl $12, %esp; \
	call name; \
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

TIMER(CMOSTimerIRQ)
TIMER(ApicTimerIRQ)
TIMER(PITIRQ)

HARDWARE(0)

.type ReturnToUser, @function
.align 16
ReturnToUser:
	movl 0(%esp), %eax
	testl %eax, %eax
	jz 1f
	pushl $USER_STACK
	pushl 32(%eax)
	pushl 36(%eax)
	pushl $USER_CODE
	pushl 0(%eax)
	movl 28(%eax), %ebp
	movl 24(%eax), %edi
	movl 20(%eax), %esi
	movl 16(%eax), %edx
	movl 12(%eax), %ecx
	movl 8(%eax), %ebx
	pushl 4(%eax)
	movl $USER_DATA, %eax
	movl %eax, %ds
	movl %eax, %es
	movl %eax, %fs
	movl %eax, %gs
	popl %eax
	iretl
1:
	sti
	hlt
	jmp 1b
.size ReturnToUser, . - ReturnToUser

.global SpuriousIRQWrapper
.type SpuriousIRQWrapper, @function
.align 16
SpuriousIRQWrapper:
	iretl
.size SpuriousIRQWrapper, . - SpuriousIRQWrapper

.global sysentry
.type sysentry, @function
.extern userMax
.extern kernelElfDynsym
.align 16
sysentry:
	cli
	cmpl $USER_OFFSET, (%esp)
	jb 1f
	cmpl $userMax, (%esp)
	jae 1f
	pushl %eax
	movl $KERNEL_DATA, %eax
	movl %eax, %ds
	movl %eax, %es
	movl %eax, %fs
	movl %eax, %gs
	movl 24(%esp), %eax
	movl %ebx, 8(%eax)
	popl %ebx
	movl %ebx, 4(%eax)
	movl %ecx, 12(%eax)
	movl %edx, 16(%eax)
	movl %esi, 20(%eax)
	movl %edi, 24(%eax)
	movl %ebp, 28(%eax)
	movl 12(%esp), %ebx
	movl %ebx, 32(%eax)
	movl 8(%esp), %ebx
	movl %ebx, 36(%eax)
	movl 0(%esp), %ebx
	movl %ebx, 0(%eax)
	addl $20, %esp
	subl $USER_OFFSET, %ebx
	andl $0xfffffff0, %ebx
	movl (kernelElfDynsym + 4)(%ebx), %eax
	call *%eax
	jmp ReturnToUser

1:
	iretl
.size sysentry, . - sysentry

.section .user
.rept 256
	.align 16
	int $0xf0
	ret
.endr
