OUTPUT_FORMAT(elf64-littleaarch64)
OUTPUT_ARCH(aarch64)
ENTRY(Entry)

kernelOffset = 0xffffffffc0000000;
kernelStart = kernelOffset + 0x8000;
initStart = kernelStart;

GROUP(../../arch/arm/v8/libv8.a ../../arch/arm/a64/liba64.a ../../arch/arm/libarm.a ../../libkernel.a libraspi.a)
INPUT(libgcc.a)

PHDRS
{
	init PT_LOAD;
	kernel PT_LOAD;
}

SECTIONS
{
	.init.text (initStart + SIZEOF_HEADERS) : AT(ADDR(.init.text) - kernelOffset)
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
	}
}

