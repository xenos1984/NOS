// Port.h - x86 I/O port communication.

#ifndef __ARCH_X86_PORT_H__
#define __ARCH_X86_PORT_H__

#include <cstdint>

namespace Kernel
{
	/** x86 I/O port. */
	class Port
	{
	private:
		const uint16_t port; /**< Port number. */

	public:
		inline Port(uint16_t n) : port(n) {}
		inline ~Port(void) {}

		inline uint8_t ReadU8(void)
		{
			uint8_t ret;
			asm volatile ("inb %1, %0" : "=a"(ret) : "dN"(port));
			return(ret);
		}

		inline uint16_t ReadU16(void)
		{
			uint16_t ret;
			asm volatile ("inw %1, %0" : "=a"(ret) : "dN"(port));
			return(ret);
		}

		inline uint32_t ReadU32(void)
		{
			uint32_t ret;
			asm volatile ("inl %1, %0" : "=a"(ret) : "dN"(port));
			return(ret);
		}

		inline void WriteU8(uint8_t value)
		{
			asm volatile ("outb %0, %1" :: "a"(value), "dN"(port));
		}

		inline void WriteU16(uint16_t value)
		{
			asm volatile ("outw %0, %1" :: "a"(value), "dN"(port));
		}

		inline void WriteU32(uint32_t value)
		{
			asm volatile ("outl %0, %1" :: "a"(value), "dN"(port));
		}

		inline int8_t ReadS8(void)
		{
			int8_t ret;
			asm volatile ("inb %1, %0" : "=a"(ret) : "dN"(port));
			return(ret);
		}

		inline int16_t ReadS16(void)
		{
			int16_t ret;
			asm volatile ("inw %1, %0" : "=a"(ret) : "dN"(port));
			return(ret);
		}

		inline int32_t ReadS32(void)
		{
			int32_t ret;
			asm volatile ("inl %1, %0" : "=a"(ret) : "dN"(port));
			return(ret);
		}

		inline void WriteS8(int8_t value)
		{
			asm volatile ("outb %0, %1" :: "a"(value), "dN"(port));
		}

		inline void WriteS16(int16_t value)
		{
			asm volatile ("outw %0, %1" :: "a"(value), "dN"(port));
		}

		inline void WriteS32(int32_t value)
		{
			asm volatile ("outl %0, %1" :: "a"(value), "dN"(port));
		}

		inline static uint8_t ReadU8(uint16_t port)
		{
			uint8_t ret;
			asm volatile ("inb %1, %0" : "=a"(ret) : "dN"(port));
			return(ret);
		}

		inline static uint16_t ReadU16(uint16_t port)
		{
			uint16_t ret;
			asm volatile ("inw %1, %0" : "=a"(ret) : "dN"(port));
			return(ret);
		}

		inline static uint32_t ReadU32(uint16_t port)
		{
			uint32_t ret;
			asm volatile ("inl %1, %0" : "=a"(ret) : "dN"(port));
			return(ret);
		}

		inline static void WriteU8(uint16_t port, uint8_t value)
		{
			asm volatile ("outb %0, %1" :: "a"(value), "dN"(port));
		}

		inline static void WriteU16(uint16_t port, uint16_t value)
		{
			asm volatile ("outw %0, %1" :: "a"(value), "dN"(port));
		}

		inline static void WriteU32(uint16_t port, uint32_t value)
		{
			asm volatile ("outl %0, %1" :: "a"(value), "dN"(port));
		}

		inline static int8_t ReadS8(uint16_t port)
		{
			int8_t ret;
			asm volatile ("inb %1, %0" : "=a"(ret) : "dN"(port));
			return(ret);
		}

		inline static int16_t ReadS16(uint16_t port)
		{
			int16_t ret;
			asm volatile ("inw %1, %0" : "=a"(ret) : "dN"(port));
			return(ret);
		}

		inline static int32_t ReadS32(uint16_t port)
		{
			int32_t ret;
			asm volatile ("inl %1, %0" : "=a"(ret) : "dN"(port));
			return(ret);
		}

		inline static void WriteS8(uint16_t port, int8_t value)
		{
			asm volatile ("outb %0, %1" :: "a"(value), "dN"(port));
		}

		inline static void WriteS16(uint16_t port, int16_t value)
		{
			asm volatile ("outw %0, %1" :: "a"(value), "dN"(port));
		}

		inline static void WriteS32(uint16_t port, int32_t value)
		{
			asm volatile ("outl %0, %1" :: "a"(value), "dN"(port));
		}
	};
}

#endif
