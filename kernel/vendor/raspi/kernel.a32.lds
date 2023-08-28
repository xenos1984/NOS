OUTPUT_FORMAT(elf32-littlearm)
OUTPUT_ARCH(arm)
ENTRY(Entry)

loadAddress = 0x8000;
kernelOffset = 0xc0000000;
kernelStart = kernelOffset;
initStart = kernelStart;

GROUP(../../arch/arm/a32/liba32.a ../../arch/arm/libarm.a ../../libkernel.a libraspi.a)
INPUT(libgcc.a)

PHDRS
{
	init PT_LOAD;
	kernel PT_LOAD;
}

SECTIONS
{
	.init.text (initStart) : AT(ADDR(.init.text) + loadAddress - kernelOffset)
	{
		initTextStart = .;
		*(.init.text)
		. = ALIGN(4K);
		initTextEnd = .;
	} :init

	.init.data : AT(ADDR(.init.data) + loadAddress - kernelOffset)
	{
		initDataStart = .;
		*(.init.data)
		. = ALIGN(4K);
		initDataEnd = .;
	}
	initEnd = .;

	/*. += SIZEOF_HEADERS;*/

	textStart = .;
	.text : AT(ADDR(.text) + loadAddress - kernelOffset)
	{
		*(.text)
		*(.text.*)
		*(.stub)
		*(.rodata*)
		*(.gnu.linkonce.t.*)
		*(.gnu.linkonce.r.*)
		. = ALIGN(4K);
	} :kernel
	textEnd = .;

	dataStart = .;
	.data : AT(ADDR(.data) + loadAddress - kernelOffset)
	{
		*(.data*)
		*(.gnu.linkonce.d.*)
	}

	dataEnd = .;
	bssStart = .;

	.bss : AT(ADDR(.bss) + loadAddress - kernelOffset)
	{
		*(.common)
		*(.bss*)
		*(.gnu.linkonce.b.*)
		. = ALIGN(4K);
		*(.stack)
		bspStack = .;
	}
	bssEnd = .;

	. = ALIGN(16K);
	.tables : AT(ADDR(.tables) + loadAddress - kernelOffset)
	{
		tabPGDIR = .;
		. += 16K;
		tabPGTAB = .;
		. += 4K;
	}
	. = ALIGN(4K);
	kernelEnd = .;

	/DISCARD/ :
	{
		*(.gcc_except_table)
		*(.eh_frame)
		*(.note)
		*(.comment)
		*(.rel.*)
		*(.rela.*)
		*(.ARM.*)
	}
}
