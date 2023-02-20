// InterruptStub.S - ARM interrupt / exception entry points.

.globl ExceptionVectorBase

.section .text
.align 11
ExceptionVectorBase:

// Synchronous EL1t
.align 7
b .

// IRQ EL1t
.align 7
b .

// FIQ EL1t
.align 7
b .

// Error EL1t
.align 7
b .

// Synchronous EL1h
.align 7
sub sp, sp, #256
stp x0, x1, [sp]
stp x2, x3, [sp, #16]
stp x4, x5, [sp, #32]
stp x6, x7, [sp, #48]
stp x8, x9, [sp, #64]
stp x10, x11, [sp, #80]
stp x12, x13, [sp, #96]
stp x14, x15, [sp, #112]
stp x16, x17, [sp, #128]
stp x18, x19, [sp, #144]
stp x20, x21, [sp, #160]
stp x22, x23, [sp, #176]
stp x24, x25, [sp, #192]
stp x26, x27, [sp, #208]
stp x28, x29, [sp, #224]
add x0, sp, #256
stp x30, x0, [sp, #240]
sub x0, x0, #256
bl KernelSyncHandler
b .

// IRQ EL1h
.align 7
b .

// FIQ EL1h
.align 7
b .

// Error EL1h
.align 7
b .

// Synchronous EL0 64 bit
.align 7
b .

// IRQ EL0 64 bit
.align 7
b .

// FIQ EL0 64 bit
.align 7
b .

// Error EL0 64 bit
.align 7
b .

// Synchronous EL0 32 bit
.align 7
b .

// IRQ EL0 32 bit
.align 7
b .

// FIQ EL0 32 bit
.align 7
b .

// Error EL0 32 bit
.align 7
b .
