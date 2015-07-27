OUTPUT_FORMAT(binary)
OUTPUT_ARCH(m68k)

intvects = 0x000000;
sysvars  = 0x000420;
ROMStart = 0xfc0000;
ROMEnd   = 0xff0000;

GROUP(libatari.a ../libm68k.a ../../../libkernel.a ../../../../fonts/libfonts.a libgcc.a)

MEMORY
{
	RAM (w!i) : ORIGIN = 0x1000, LENGTH = 0xdff000
	CART (ir) : ORIGIN = 0xfa0000, LENGTH = 0x020000
	ROM (ir) : ORIGIN = 0xfc0000, LENGTH = 0x030000
	IO (w!i) : ORIGIN = 0xff0000, LENGTH = 0x010000
}

SECTIONS
{
	rom :
	{
		*(.header)
		*(.init.text)
		*(.text)
		*(.text.*)
		*(.stub)
		*(.rodata*)
		*(.init.data)
		*(.gnu.linkonce.t.*)
		*(.gnu.linkonce.r.*)
		. = ROMEnd - ROMStart;
	} > ROM = 0xa5a5a5a5

	ram :
	{
		*(.bss)
	} > RAM

	/DISCARD/ :
	{
		*(.data)
		*(.gcc_except_table)
		*(.eh_frame)
		*(.note)
		*(.comment)
		*(.rel.*)
		*(.rela.*)
	}
}
