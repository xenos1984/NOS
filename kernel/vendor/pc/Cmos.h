// Cmos.h - Support for MC146818 CMOS RAM / Real Time Clock.

#ifndef __VENDOR_PC_CMOS_H__
#define __VENDOR_PC_CMOS_H__

#include <cstdint>
#include <Clock.h>
#include <SpinLock.h>
#include INC_ARCH(Port.h)

#define READ_WRITE_REG(name, reg) \
	inline uint8_t Get##name(void) \
	{ \
		return ReadRegister(reg); \
	} \
	\
	inline void Set##name(uint8_t value) \
	{ \
		WriteRegister(reg, value); \
	}

namespace Kernel
{
	/**
	 * Real Time Clock / CMOS chip.
	 * The RTC / CMOS chip provides time and configuration data. It can be accessed via ports 70h and 71h.
	 */
	namespace Cmos
	{
		enum RegIndex : uint8_t
		{
			REG_SEC      = 0x00, /**< Seconds */
			REG_AL_SEC   = 0x01, /**< Alarm seconds */
			REG_MIN      = 0x02, /**< Minutes */
			REG_AL_MIN   = 0x03, /**< Alarm minutes */
			REG_HOUR     = 0x04, /**< Hours */
			REG_AL_HOUR  = 0x05, /**< Alarm hours */
			REG_DOW      = 0x06, /**< Day of week */
			REG_DAY      = 0x07, /**< Day of month */
			REG_MONTH    = 0x08, /**< Month */
			REG_YEAR     = 0x09, /**< Year */
			REG_STATUS_A = 0x0a, /**< Status A */
			REG_STATUS_B = 0x0b, /**< Status B */
			REG_STATUS_C = 0x0c, /**< Status C */
			REG_STATUS_D = 0x0d, /**< Status D */
			REG_POST     = 0x0e, /**< POST status */
			REG_SHUTDOWN = 0x0f, /**< Shutdown status */
			REG_CENT     = 0x32  /**< Century */
		};

		enum RegAddr : uint8_t
		{
			CMOS_ADDR = 0x70,
			CMOS_DATA = 0x71
		};

		uint8_t ReadRegister(uint8_t reg);
		void WriteRegister(uint8_t reg, uint8_t value);

		void Init(void);

		enum RegValue : uint8_t
		{
			// Shutdown status codes
			SH_NORMAL     = 0x00, /**< Normal */
			SH_RM_INIT    = 0x01, /**< Real mode init */
			SH_JMP_BOOT   = 0x04, /**< Jump to bootstrap */
			SH_EOI_JMP467 = 0x05, /**< Signal EOI to PIC, clear keyboard, jump to [40:67h] */
			SH_RET_INT15  = 0x07, /**< Return to INT 15h, func 87h */
			SH_RET_POST   = 0x08, /**< Return to POST */
			SH_JMP467     = 0x0a, /**< Jump to [40:67h] */
			SH_IRET467    = 0x0b, /**< IRET to [40:67h] */
			SH_RET467     = 0x0c, /**< Return to [40:67h] */

			// POST status codes
			POST_POWER    = 0x80, /**< Power failure */
			POST_CHECKSUM = 0x40, /**< Checksum OK */
			POST_CONFIG   = 0x20, /**< Invalid config */
			POST_MEMDIFF  = 0x10, /**< Memory size different from entry */
			POST_HDD      = 0x08, /**< HDD init failure */
			POST_CLOCK    = 0x04, /**< RTC failure */

			// Status register A
			A_UPDATE      = 0x80, /**< Update in progress */

			// Status register B
			B_HALTED      = 0x80, /**< RTC halted */
			B_SQUARE      = 0x08, /**< Enable square wave output */
			B_BCD         = 0x04, /**< Time and date BCD coded */
			B_24H         = 0x02, /**< 24 hour mode */
			B_DAYLIGHT    = 0x01, /**< Daylight saving time */

			// Status register B & C interrupt flags
			IRQ_PENDING   = 0x80, /**< IRQ pending */
			IRQ_PER       = 0x40, /**< Periodic interrupt */
			IRQ_ALARM     = 0x20, /**< Alarm interrupt */
			IRQ_UPDATE    = 0x10, /**< Update interrupt */

			// Status register D
			D_VALID       = 0x80  /**< RTC data valid */
		};

		READ_WRITE_REG(Second, REG_SEC)
		READ_WRITE_REG(Minute, REG_MIN)
		READ_WRITE_REG(Hour, REG_HOUR)
		READ_WRITE_REG(DayOfWeek, REG_DOW)
		READ_WRITE_REG(Day, REG_DAY)
		READ_WRITE_REG(Month, REG_MONTH)
		READ_WRITE_REG(Year, REG_YEAR)
		READ_WRITE_REG(Century, REG_CENT)

		READ_WRITE_REG(StatusA, REG_STATUS_A)
		READ_WRITE_REG(StatusB, REG_STATUS_B)
		READ_WRITE_REG(StatusC, REG_STATUS_C)
		READ_WRITE_REG(StatusD, REG_STATUS_D)

		READ_WRITE_REG(ShutdownStatus, REG_SHUTDOWN)

		Time GetTime(void);
		void WaitForSecond(void);
		void SetPeriodic(uint8_t div); /**< Periodic interrupt with frequency 2Hz * 2^(15 - div) */

		extern SpinLock lock;
	}
}

#undef READ_WRITE_REG

#endif
