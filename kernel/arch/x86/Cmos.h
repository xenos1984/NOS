// Cmos.h - Support for MC146818 CMOS RAM / Real Time Clock.

#ifndef __ARCH_X86_CMOS_H__
#define __ARCH_X86_CMOS_H__

#include <Clock.h>
#include <AtomicOps.h>
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
	class Cmos
	{
	private:
		static const uint8_t REG_SEC      = 0x00; /**< Seconds */
		static const uint8_t REG_AL_SEC   = 0x01; /**< Alarm seconds */
		static const uint8_t REG_MIN      = 0x02; /**< Minutes */
		static const uint8_t REG_AL_MIN   = 0x03; /**< Alarm minutes */
		static const uint8_t REG_HOUR     = 0x04; /**< Hours */
		static const uint8_t REG_AL_HOUR  = 0x05; /**< Alarm hours */
		static const uint8_t REG_DOW      = 0x06; /**< Day of week */
		static const uint8_t REG_DAY      = 0x07; /**< Day of month */
		static const uint8_t REG_MONTH    = 0x08; /**< Month */
		static const uint8_t REG_YEAR     = 0x09; /**< Year */
		static const uint8_t REG_STATUS_A = 0x0a; /**< Status A */
		static const uint8_t REG_STATUS_B = 0x0b; /**< Status B */
		static const uint8_t REG_STATUS_C = 0x0c; /**< Status C */
		static const uint8_t REG_STATUS_D = 0x0d; /**< Status D */
		static const uint8_t REG_POST     = 0x0e; /**< POST status */
		static const uint8_t REG_SHUTDOWN = 0x0f; /**< Shutdown status */
		static const uint8_t REG_CENT     = 0x32; /**< Century */

		AtomicLock lock;
		static const uint8_t CMOS_ADDR = 0x70, CMOS_DATA = 0x71;

		uint8_t ReadRegister(uint8_t reg);
		void WriteRegister(uint8_t reg, uint8_t value);

	public:
		Cmos(void);

		// Shutdown status codes
		static const uint8_t SH_NORMAL     = 0x00; /**< Normal */
		static const uint8_t SH_RM_INIT    = 0x01; /**< Real mode init */
		static const uint8_t SH_JMP_BOOT   = 0x04; /**< Jump to bootstrap */
		static const uint8_t SH_EOI_JMP467 = 0x05; /**< Signal EOI to PIC, clear keyboard, jump to [40:67h] */
		static const uint8_t SH_RET_INT15  = 0x07; /**< Return to INT 15h, func 87h */
		static const uint8_t SH_RET_POST   = 0x08; /**< Return to POST */
		static const uint8_t SH_JMP467     = 0x0a; /**< Jump to [40:67h] */
		static const uint8_t SH_IRET467    = 0x0b; /**< IRET to [40:67h] */
		static const uint8_t SH_RET467     = 0x0c; /**< Return to [40:67h] */

		// POST status codes
		static const uint8_t POST_POWER    = 0x80; /**< Power failure */
		static const uint8_t POST_CHECKSUM = 0x40; /**< Checksum OK */
		static const uint8_t POST_CONFIG   = 0x20; /**< Invalid config */
		static const uint8_t POST_MEMDIFF  = 0x10; /**< Memory size different from entry */
		static const uint8_t POST_HDD      = 0x08; /**< HDD init failure */
		static const uint8_t POST_CLOCK    = 0x04; /**< RTC failure */

		// Status register A
		static const uint8_t A_UPDATE      = 0x80; /**< Update in progress */

		// Status register B
		static const uint8_t B_HALTED      = 0x80; /**< RTC halted */
		static const uint8_t B_SQUARE      = 0x08; /**< Enable square wave output */
		static const uint8_t B_BCD         = 0x04; /**< Time and date BCD coded */
		static const uint8_t B_24H         = 0x02; /**< 24 hour mode */
		static const uint8_t B_DAYLIGHT    = 0x01; /**< Daylight saving time */

		// Status register B & C interrupt flags
		static const uint8_t IRQ_PENDING   = 0x80; /**< IRQ pending */
		static const uint8_t IRQ_PER       = 0x40; /**< Periodic interrupt */
		static const uint8_t IRQ_ALARM     = 0x20; /**< Alarm interrupt */
		static const uint8_t IRQ_UPDATE    = 0x10; /**< Update interrupt */

		// Status register D
		static const uint8_t D_VALID       = 0x80; /**< RTC data valid */

		READ_WRITE_REG(Second, REG_SEC);
		READ_WRITE_REG(Minute, REG_MIN);
		READ_WRITE_REG(Hour, REG_HOUR);
		READ_WRITE_REG(DayOfWeek, REG_DOW);
		READ_WRITE_REG(Day, REG_DAY);
		READ_WRITE_REG(Month, REG_MONTH);
		READ_WRITE_REG(Year, REG_YEAR);
		READ_WRITE_REG(Century, REG_CENT);

		READ_WRITE_REG(StatusA, REG_STATUS_A);
		READ_WRITE_REG(StatusB, REG_STATUS_B);
		READ_WRITE_REG(StatusC, REG_STATUS_C);
		READ_WRITE_REG(StatusD, REG_STATUS_D);

		READ_WRITE_REG(ShutdownStatus, REG_SHUTDOWN);

		Time GetTime(void);
		void WaitForSecond(void);
		void SetPeriodic(uint8_t div); /**< Periodic interrupt with frequency 2Hz * 2^(15 - div) */
	};
}

extern char cmos_space[];
inline Kernel::Cmos& cmos(void) { return(reinterpret_cast<Kernel::Cmos&>(cmos_space)); }

#undef READ_WRITE_REG

#endif
