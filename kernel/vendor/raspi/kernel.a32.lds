OUTPUT_FORMAT(elf32-littlearm)
OUTPUT_ARCH(arm)
ENTRY(Entry)

kernelOffset = 0xc0000000;
kernelStart = kernelOffset + 0x8000;
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
	.init.text (initStart) : AT(ADDR(.init.text) - kernelOffset)
	{
		initTextStart = .;
		*(.init.text)
		. = ALIGN(4K);
		initTextEnd = .;
	} :init

	.init.data : AT(ADDR(.init.data) - kernelOffset)
	{
		initDataStart = .;
		*(.init.data)
		. = ALIGN(4K);
		initDataEnd = .;
	}
	initEnd = .;

	/*. += SIZEOF_HEADERS;*/

	.text : AT(ADDR(.text) - kernelOffset)
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
	.data : AT(ADDR(.data) - kernelOffset)
	{
		*(.data*)
		*(.gnu.linkonce.d.*)
	}

	dataEnd = .;
	bssStart = .;

	.bss : AT(ADDR(.bss) - kernelOffset)
	{
		*(.common)
		*(.bss*)
		*(.gnu.linkonce.b.*)
	}
	. = ALIGN(4K);
	. += 4K;
	bspStack = .;
	bssEnd = .;

	. = ALIGN(16K);
	.tables : AT(ADDR(.tables) - kernelOffset)
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
