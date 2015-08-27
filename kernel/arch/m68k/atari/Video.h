// Video.h - Atari video controller.

#ifndef __ARCH_M68K_ATARI_VIDEO_H__
#define __ARCH_M68K_ATARI_VIDEO_H__

#include <cstdint>

namespace Kernel
{
	class Video
	{
	private:
		struct _video
		{
			volatile uint8_t space1;
			volatile uint8_t vidmemhigh;
			volatile uint8_t space2;
			volatile uint8_t vidmemmid;
			volatile uint8_t space3;
			volatile uint8_t vidcounthigh;
			volatile uint8_t space4;
			volatile uint8_t vidcountmid;
			volatile uint8_t space5;
			volatile uint8_t vidcountlow;
			volatile uint8_t syncmode;
			volatile uint8_t space6[53];
			volatile uint16_t color[16];
			volatile uint8_t shiftmode;
		} PACKED;

		inline static _video& _v(void) { return(*(_video*)0xff8200); };

	public:
		static void Init(void);
		static void SetShiftMode(uint8_t mode);
		static uint8_t GetShiftMode(void);
		static void SetVideoMemory(void* mem);
		static void* GetVideoMemory(void);
		static uint32_t GetCount(void);
		static void SetPalette(uint8_t index, uint16_t value);
		static void SetPalette(const uint16_t* values);
	};
}

#endif
