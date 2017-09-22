// InterruptStub.S - ARM interrupt / exception entry points.

.align 5
ExceptionVectorBase:
b . // Reset
b . // Undefined instruction
b . // Software interrupt
b . // Prefetch abort
b . // Data abort
b . // Hypervisor trap
b . // IRQ
b . // FIQ - no need to branch, just continue here.

