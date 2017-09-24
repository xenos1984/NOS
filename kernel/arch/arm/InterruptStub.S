// InterruptStub.S - ARM interrupt / exception entry points.

.globl ExceptionVectorBase

.section .text
.align 5
ExceptionVectorBase:
b . // Reset
b . // Undefined instruction
b . // Software interrupt
b . // Prefetch abort
b DataAbortWrapper // Data abort
b . // Hypervisor trap
b . // IRQ
b . // FIQ - no need to branch, just continue here.

DataAbortWrapper:
sub lr, #8
srsdb #0x13!
cpsid i, #0x13
push {r0-r12, lr}
sub sp, #8
stmia sp, {r13, r14}^
mov r0, sp
tst sp, #4
subeq sp, #4
push {r0}

bl DataAbort
b .

pop {r0}
mov sp, r0
ldmia sp, {r13, r14}^
add sp, #8
pop {r0-r3, r12, lr}
rfeia sp!

