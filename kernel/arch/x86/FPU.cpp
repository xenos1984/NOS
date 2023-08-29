// FPU.cpp - Basic FPU functions.

#include <new>
#include INC_ARCH(FPU.h)
#include INC_ARCH(CPU.h)
#include <Heap.h>

using namespace Kernel;

FPUState::FPUState(void)
{
	ControlWord.raw = 0x037f;
	StatusWord.raw = 0x0;
	TagWord = 0xffff;
}

#ifdef ARCH_X86_X32
SSEState::SSEState(void)
{
	ControlWord.raw = 0x037f;
	StatusWord.raw = 0x0;
	TagWord = 0xff;
}
#endif
#ifdef ARCH_X86_X64
SSEState32::SSEState32(void)
{
	ControlWord.raw = 0x037f;
	StatusWord.raw = 0x0;
	TagWord = 0xff;
}

SSEState64::SSEState64(void)
{
	ControlWord.raw = 0x037f;
	StatusWord.raw = 0x0;
	TagWord = 0xff;
}
#endif

AVXState::AVXState(void)
{
	XState = 7;
}

MMState::MMState(void)
{
	if(bspcpu().HasXSR())
		new (&avx) AVXState;
	else if(bspcpu().HasFXSR())
		new (&sse) SSEState;
	else if(bspcpu().HasFPU())
		new (&fpu) FPUState;
}

void MMState::Save(void)
{
	if(bspcpu().HasXSR())
		avx.Save();
	else if(bspcpu().HasFXSR())
		sse.Save();
	else if(bspcpu().HasFPU())
		fpu.Save();
}

void MMState::Restore(void)
{
	if(bspcpu().HasXSR())
		avx.Restore();
	else if(bspcpu().HasFXSR())
		sse.Restore();
	else if(bspcpu().HasFPU())
		fpu.Restore();
}

MMState* MMState::Create(void)
{
	MMState* mms = (MMState*)(Heap::Alloc(bspcpu().GetSaveAreaSize(), 16, true));

	if(bspcpu().HasXSR())
		new (mms) AVXState;
	else if(bspcpu().HasFXSR())
		new (mms) SSEState;
	else if(bspcpu().HasFPU())
		new (mms) FPUState;

	return(mms);
}
