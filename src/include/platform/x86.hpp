#pragma once

#ifdef __GNUC__
#define PACKED __attribute__((packed))
#define CDECL __attribute__((cdecl))
#else
#define PACKED
#define CDECL
#endif


extern "C" void CDECL outportb(unsigned short port, unsigned char value);
extern "C" unsigned char CDECL inportb(unsigned short port);