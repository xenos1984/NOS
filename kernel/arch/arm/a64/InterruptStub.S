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

