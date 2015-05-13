// InterruptStub.S - Assembler part of interrupt routines

#define INTERRUPT(name) \
.global name##Wrapper; \
.type name##Wrapper, @function; \
.extern name; \
name##Wrapper: \
	moveml %d0-%d7/%a0-%a6, -(%sp); \
	movel %usp, %a6; \
	movel %a6, -(%sp); \
	peal (%sp); \
	bsrw name; \
	addqw #4, %sp; \
	moveal (%sp)+, %a6; \
	movel %a6, %usp; \
	moveml (%sp)+, %d0-%d7/%a0-%a6; \
	rte; \
.size name##Wrapper, . - name##Wrapper

INTERRUPT(BusError)
INTERRUPT(AddressError)
INTERRUPT(IllegalInstr)
INTERRUPT(DivByZero)
