OUTPUT_FORMAT(elf64-littleaarch64)
OUTPUT_ARCH(aarch64)
ENTRY(Entry)

loadAddress = 0x80000;
kernelOffset = 0xffffffffc0000000;
kernelStart = kernelOffset;
initStart = kernelStart;

GROUP(../../arch/arm/a64/liba64.a ../../arch/arm/libarm.a ../../libkernel.a libraspi.a)
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
	}
	. = ALIGN(4K);
	. += 4K;
	bspStack = .;
	bssEnd = .;

	. = ALIGN(16K);
	.tables : AT(ADDR(.tables) + loadAddress - kernelOffset)
	{
		tabPGL1 = .;
		. += 4K;
		tabPGL2 = .;
		. += 4K;
		tabPGL3 = .;
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
