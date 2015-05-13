// ROMEntry.S - Entry point for ROM version of the kernel.

.extern KernelEntry

.section .header
.type header, @object
header:
	bras Entry
	.word 0x0100
	.long Entry
	.long header
	.long 0
	.long Entry
	.long 0
	.long 0
	.word 0
	.word 0
	.long 0
	.long 0
	.long 0
	.ascii "ETOS"
.size header, . - header

.section .init.text
.type Entry, @function
Entry:
	movew #0x2700, %sr // Disable interrupts.
	reset // Reset hardware.

	// Check for diagnostic cartridge.
	cmpl #0xfa52235f, 0xfa0000
	bnes 1f

	lea 1f, %a6
	jmp 0xfa0004

1:
	// Check for valid memory info
	cmpl #0x752019f3, 0x420
	bnes 2f
	cmpl #0x237698aa, 0x43a
	bnes 2f
	cmpl #0x5555aaaa, 0x51a
	bnes 2f

	// Valid! Restore bank settings.
	moveb 0x42e, 0xffff8001
	braw 5f

2:
	// Determine installed memory size.
	moveb #0x0a, 0xffff8001 // Assume 2MB in both banks.
	lea 0x00000008, %a0 // Start of bank 0.
	lea 0x00200008, %a1 // Start of bank 1.
	movew #0xabcd, %d0 // Some bitpattern.
	movew %d0, (%a0)
	movew %d0, (%a1)

	clrl %d5 // Set total memory to 0.
	clrl %d6 // Both banks set to 128k.
	movel #0x00200000, %d1 // Bank 1.

3:
	lsrw #0x02, %d6 // Shift bank config.

	addl #0x20000, %d5 // Add 128k to total mem.
	moveal %d1, %a0 // Load bank address.
	adda #0x208, %a0 // Add offset.
	cmpw (%a0), %d0 // Pattern found?
	beqs 4f // Yes! 128k.

	addl #0x60000, %d5 // Add 512k - 128k to total mem.
	addqb #0x04, %d6 // Increase bank config.
	moveal %d1, %a0 // Load bank address.
	adda #0x408, %a0 // Add offset.
	cmpw (%a0), %d0 // Pattern found?
	beqs 4f // Yes! 512k.

	addl #0x180000, %d5 // Add 2048k - 512k to total mem.
	addqb #0x04, %d6 // Increase bank config.
	moveal %d1, %a0 // Load bank address.
	adda #0x008, %a0 // Add offset.
	cmpw (%a0), %d0 // Pattern found?
	beqs 4f // Yes! 2048k.

	// Bank is empty! Don't know how to handle this.
	stop #0x2700
	bras .

4:
	subl #0x00200000, %d1 // Switch bank.
	beqs 3b // Bank 0? Continue memory test.

	moveb %d6, (0xffff8001) // Set memory controller.
	moveb %d6, (0x424) // Save a copy.
	movel %d5, (0x42e) // Save top of RAM.

	// set magic values
	movel #0x752019f3, (0x420)
	movel #0x237698aa, (0x43a)
	movel #0x5555aaaa, (0x51a)

5:
	lea 0x7fc, %sp // Set initial stack pointer.
	bsrw KernelEntry // Time for some C!
	stop #0x2700 // Halt CPU with interrupts disabled.
	bras .
.size Entry, . - Entry
