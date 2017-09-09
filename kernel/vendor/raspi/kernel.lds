OUTPUT_FORMAT(elf32-littlearm)
OUTPUT_ARCH(arm)
ENTRY(Entry)

GROUP(../../libkernel.a libraspi.a)
INPUT(libgcc.a)

PHDRS
{
	init PT_LOAD;
	kernel PT_LOAD;
}

SECTIONS
{
	.init.text 0x8000 :
	{
		initTextStart = .;
		*(.init.text)
		. = ALIGN(4K);
		initTextEnd = .;
	} :init

	.init.data :
	{
		initDataStart = .;
		*(.init.data)
		. = ALIGN(4K);
		initDataEnd = .;
	}
	initEnd = .;

	.text :
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
	.data :
	{
		*(.data*)
		*(.gnu.linkonce.d.*)
	}

	dataEnd = .;
	bssStart = .;

	.bss :
	{
		*(.common)
		*(.bss*)
		*(.gnu.linkonce.b.*)
	}
	. = ALIGN(4K);
	. += 4K;
	bspStack = .;
	bssEnd = .;

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

