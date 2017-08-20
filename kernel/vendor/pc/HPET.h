// HPET.h - High Precision Event Timer

#ifndef __ARCH_X86_HPET_H__
#define __ARCH_X86_HPET_H__

#include <cstdint>
#include <AtomicOps.h>

namespace Kernel
{
	class HPET
	{
	private:
		struct _HPET
		{
			volatile uint8_t RevID;
			volatile uint64_t TimerCount : 5;
			volatile uint64_t Timer64Bit : 1;
			volatile uint64_t Reserved1 : 1;
			volatile uint64_t LegRouteCap : 1;
			volatile uint16_t VendorID;
			volatile uint32_t ClockPeriod;
			volatile uint64_t Space1;
			volatile uint64_t EnableConf : 1;
			volatile uint64_t LegRouteConf : 1;
			volatile uint64_t Reserved2 : 62;
			volatile uint64_t Space2;
			volatile uint32_t InterruptStatus;
			volatile uint32_t Reserved3;
			volatile uint64_t Space3[25];
			union
			{
				volatile uint64_t Value64;
				struct
				{
					volatile uint32_t Low;
					volatile uint32_t High;
				} Value32;
			} MainCounter;
			volatile uint64_t Space4;
			struct
			{
				volatile uint64_t Reserved4 : 1;
				volatile uint64_t IntTypeConf : 1;
				volatile uint64_t IntEnableConf : 1;
				volatile uint64_t TypeConf : 1;
				volatile uint64_t PeriodicIntConf : 1;
				volatile uint64_t SizeCap : 1;
				volatile uint64_t ValueSetConf : 1;
				volatile uint64_t Reserved5 : 1;
				volatile uint64_t Mode32Conf : 1;
				volatile uint64_t IntRouteConf : 5;
				volatile uint64_t FSBEnableConf : 1;
				volatile uint64_t FSBIntDelCap : 1;
				volatile uint16_t Reserved6;
				volatile uint32_t IntRouteCap;
				union
				{
					volatile uint64_t Value64;
					struct
					{
						volatile uint32_t Low;
						volatile uint32_t High;
					} Value32;
				} Comparator;
				volatile uint32_t FSBIntValue;
				volatile uint32_t FSBIntAddress;
				volatile uint64_t Space5;
			} Timer[32];
		} PACKED *base;
		AtomicLock lock;

		public:
		HPET(unsigned long addr);

		unsigned char GetRevision(void) const;
		unsigned char GetTimerCount(void) const;
		bool Timer64Bit(void) const;
		bool LegacyCapable(void) const;
		unsigned short GetVendor(void) const;
		unsigned int GetPeriod(void) const;

		bool InterruptActive(int n) const;
		void ClearInterrupt(int n);

		uint64_t GetCurrentCount64(void) const;
		void SetCurrentCount64(uint64_t c);
		uint32_t GetCurrentCount32(void) const;
		void SetCurrentCount32(uint32_t c);

		uint64_t GetComparator64(int n) const;
		void SetComparator64(int n, uint64_t c);
		uint32_t GetComparator32(int n) const;
		void SetComparator32(int n, uint32_t c);
	};
}

#endif
