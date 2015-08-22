// CPU.cpp - x86 CPU detection and features.

#include <Console.h>
#include INC_ARCH(CPU.h)

using namespace Kernel;

char bspcpu_space[sizeof(CPU)];

SECTION(".init.text") CPU::CPU(void)
{
	int i;

	asm volatile ( "cpuid"
		: "=a" (max_std_func), "=b" (vendor.bytes.ebx_00000000), "=c" (vendor.bytes.ecx_00000000), "=d" (vendor.bytes.edx_00000000)
		: "0" (0x00000000)
	);

	if(max_std_func >= 0x00000001)
	{
		asm volatile ( "cpuid"
			: "=a" (eax_00000001.raw), "=b" (ebx_00000001.raw), "=c" (ecx_00000001.raw), "=d" (edx_00000001.raw)
			: "0" (0x00000001)
		);
	}
	else
	{
		eax_00000001.raw = 0;
		ebx_00000001.raw = 0;
		ecx_00000001.raw = 0;
		edx_00000001.raw = 0;
	}

	Console::WriteMessage(Console::Style::INFO, "CPU Vendor:", "%12s", vendor.string);
	Console::WriteMessage(Console::Style::INFO, "CPU Type:", "Type: %1x Fam: %2x%1x Mod: %1x%1x Step: %1x", eax_00000001.bits.type, eax_00000001.bits.extfamily, eax_00000001.bits.family, eax_00000001.bits.extmodel, eax_00000001.bits.model, eax_00000001.bits.stepping);
	Console::WriteMessage(Console::Style::INFO, "CPU Features 1:", "EDX = 0x%8x, ECX = 0x%8x", edx_00000001.raw, ecx_00000001.raw);

	asm volatile ( "cpuid"
		: "=a" (max_ext_func)
		: "0" (0x80000000)
		: "ebx", "ecx", "edx"
	);

	if(max_ext_func >= 0x80000001)
	{
		asm volatile ( "cpuid"
			: "=b" (ebx_80000001.raw), "=c" (ecx_80000001.raw), "=d" (edx_80000001.raw)
			: "a" (0x80000001)
		);
		Console::WriteMessage(Console::Style::INFO, "CPU Features 2:", "EDX = 0x%8x, ECX = 0x%8x", edx_80000001.raw, ecx_80000001.raw);
	}
	else
	{
		ebx_80000001.raw = 0;
		ecx_80000001.raw = 0;
		edx_80000001.raw = 0;
	}

	if(max_ext_func >= 0x80000004)
	{
		asm volatile ( "cpuid"
			: "=a" (brand.bytes.eax_80000002), "=b" (brand.bytes.ebx_80000002), "=c" (brand.bytes.ecx_80000002), "=d" (brand.bytes.edx_80000002)
			: "0" (0x80000002)
		);
		asm volatile ( "cpuid"
			: "=a" (brand.bytes.eax_80000003), "=b" (brand.bytes.ebx_80000003), "=c" (brand.bytes.ecx_80000003), "=d" (brand.bytes.edx_80000003)
			: "0" (0x80000003)
		);
		asm volatile ( "cpuid"
			: "=a" (brand.bytes.eax_80000004), "=b" (brand.bytes.ebx_80000004), "=c" (brand.bytes.ecx_80000004), "=d" (brand.bytes.edx_80000004)
			: "0" (0x80000004)
		);
		Console::WriteMessage(Console::Style::INFO, "CPU Brand:", "%s", brand.string);
	}
	else
	{
		for(i = 0; i < 48; i++)
			brand.string[i] = 0;
	}

	if(HasXSR())
	{
		asm volatile ( "cpuid"
			: "=a" (eax_0000000d), "=b" (ebx_0000000d), "=c" (ecx_0000000d), "=d" (edx_0000000d)
			: "0" (0x0000000d), "2" (0x00000000)
		);
	}
	else
	{
		eax_0000000d = 0;
		ebx_0000000d = 0;
		edx_0000000d = 0;
		if(HasFXSR())
			ecx_0000000d = 512;
		else if(HasFPU())
			ecx_0000000d = 108;
		else
			ecx_0000000d = 0;
	}
}
