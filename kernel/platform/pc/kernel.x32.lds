OUTPUT_FORMAT(elf32-i386)
OUTPUT_ARCH(i386)
ENTRY(Entry)

userOffset   = 0x80000000;
kernelOffset = 0xc0000000;
heapStart    = 0xd0000000;
heapTab      = 0xdf000000;
heapEnd      = 0xe0000000;

kernelStart = kernelOffset + 0x100000;
initStart = kernelStart;
kernelElfHeader = kernelStart;

mb_magic     = 0x1badb002;
mb_flags     = 0x00010003;
mb_checksum  = -(mb_magic + mb_flags);

GROUP(../../arch/x86/x32/libx32.a ../../arch/x86/libx86.a ../../libkernel.a libpc.a)
INPUT(libgcc.a)

PHDRS
{
	init PT_LOAD FILEHDR;
	user PT_LOAD;
	kernel PT_LOAD;
	dynamic PT_DYNAMIC;
}

SECTIONS
{
	.init.text (initStart + SIZEOF_HEADERS) : AT(ADDR(.init.text) - kernelOffset)
	{
		. = ALIGN(16);
		mb_address = .;
		LONG(mb_magic);
		LONG(mb_flags);
		LONG(mb_checksum);
		LONG(mb_address - kernelOffset);
		LONG(initStart - kernelOffset);
		LONG(dataEnd - kernelOffset);
		LONG(kernelEnd - kernelOffset);
		LONG(Entry - kernelOffset);

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
	} :init
	initEnd = .;

	userStart = .;
	.user (userOffset) : AT(userStart - kernelOffset)
	{
		*(.user)
		. = ALIGN(4K);
		userMax = .;
	} :user
	userEnd = userStart + SIZEOF(.user);

	kernelElfDynsym = userEnd;
	textStart = userEnd;
	.dynsym (userEnd) : AT(ADDR(.dynsym) - kernelOffset) { *(.dynsym) } :kernel
	kernelElfDynstr = .;
	.dynstr : AT(ADDR(.dynstr) - kernelOffset) { *(.dynstr) }
	kernelElfHash = .;
	.hash : AT(ADDR(.hash) - kernelOffset) { *(.hash) }

	.text : AT(ADDR(.text) - kernelOffset)
	{
		*(.text)
		*(.text.*)
		*(.stub)
		*(.rodata*)
		*(.gnu.linkonce.t.*)
		*(.gnu.linkonce.r.*)
		. = ALIGN(4K);
	}
	textEnd = .;

	dataStart = .;
	.data : AT(ADDR(.data) - kernelOffset)
	{
		*(.data*)
		*(.gnu.linkonce.d.*)
	}

	kernelElfDynamic = .;
	.dynamic : { *(.dynamic) } :kernel :dynamic
	.got.plt : { *(.got.plt) } :kernel

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

	.tables : AT(ADDR(.tables) - kernelOffset)
	{
		apic_base = .;
		. += 4K;
		tabIDT = .;
		. += 2K;
		tabGDT = .;
		. += 2K;
		tabPGDIR = .;
		. += 4K;
		tabPGTAB = .;
		. += 4K;
		tabBITS = .;
		. += 128K;
	}
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
