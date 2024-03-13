// AML.h - ACPI Machine Language

#ifndef __PLATFORM_PC_AML_H__
#define __PLATFORM_PC_AML_H__

#include INC_PLATFORM(ACPI.h)

#ifdef CONFIG_ACPI
namespace Kernel
{
	class AML
	{
	public:
		static void ParseTable(ACPI::TableHeader* header);

	private:
		static void ParseCode(unsigned char **x, unsigned int len);
		static unsigned int PackageLength(unsigned char **x);
		static void NameString(unsigned char **x);
	};
}
#endif

#endif
