// This file provides inline assembly functions for port I/O operations

#include "x86.hpp"

/*
 * Output a byte to a port
 */
void outportb(unsigned short port, unsigned char value)
{
#ifndef _MSC_VER
	asm volatile("outb %0, %1" : : "a"(value), "Nd"(port));
#else
	__asm {
		mov dx, port
		mov al, value
		out dx, al
	}
#endif
}

/*
 * Input a byte from a port
 * Returns the byte read from the specified port
 */
unsigned char inportb(unsigned short port)
{
	unsigned char ret;
#ifndef _MSC_VER
	asm volatile("inb %1, %0" : "=a"(ret) : "Nd"(port));
#else
	__asm {
		mov dx, port
		in al, dx
		mov ret, al
	}
#endif
	return ret;
}
