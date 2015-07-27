// SysVars.h - Standard system variables.

#ifndef __ARCH_M68K_ATARI_SYSVARS_H__
#define __ARCH_M68K_ATARI_SYSVARS_H__

#include <cstdint>

extern struct _sysvars
{
	uint32_t MemValid1;
	uint16_t MemController;
	uint32_t ResValid;
	uint32_t ResVector;
	uint32_t PhysTop;
	uint32_t MemBot;
	uint32_t MemTop;
	uint32_t MemValid2;
	uint16_t DMALock;
	uint16_t SeekRate;
	uint16_t TimerMS;
	uint16_t FloppyVerify;
	uint16_t BootDevice;
	uint16_t PalMode;
	uint16_t DefShifterMode;
	uint16_t CurShifterMode;
	uint32_t VideoBase;
	uint16_t VBlankEnable;
	uint16_t VBlankLength;
	uint32_t VBlankQueue;
	uint32_t ColorPointer;
	uint32_t ScreenPointer;
	uint32_t VBlankClock;
	uint32_t VBlankClock2;
	uint32_t HarddiskInit;
	uint32_t MonitorSwitch;
	uint32_t HarddiskBPB;
	uint32_t HarddiskRW;
	uint32_t HarddiskBoot;
	uint32_t HarddiskMedia;
	uint16_t CmdLoad;
	uint8_t TermConfig;
	uint8_t Reserved;
	uint32_t Trap14Ret;
	uint32_t CriticalRet;
	uint8_t MemDesc[16];
	uint32_t AddMemDesc;
	uint32_t SavePointer;
	uint16_t FloppyCount;
	uint32_t ConState;
	uint16_t SaveRow;
	uint32_t SaveContext;
	uint32_t Buffer[2];
	uint32_t Hz200;
	uint32_t Environmemt;
	uint32_t DriveBits;
	uint32_t DiskBuffer;
	uint32_t AutoPath;
	uint32_t VBlankList[8];
	uint16_t PrintCount;
	uint16_t PrintAbort;
	uint32_t SysBase;
	uint32_t ShellPointer;
	uint32_t EndOS;
	uint32_t ExecOS;
	uint32_t ScreenDump;
	uint32_t PrnStatus;
	uint32_t PrnOutput;
	uint32_t AuxStatus;
	uint32_t AuxOutput;
	uint32_t PunPointer;
	uint32_t MemValid3;
	uint32_t ConStat[8];
	uint32_t ConIn[8];
	uint32_t CoStat[8];
	uint32_t ConOut[8];
	uint16_t LongFrame;
	uint32_t CookieJar;
	uint32_t RamTop;
	uint32_t RamValid;
	uint32_t BellHook;
	uint32_t KeyClick;
} PACKED sysvars;

#endif
