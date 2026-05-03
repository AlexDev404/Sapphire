// debug.cpp
#include "debug.hpp"

#define DEBUG_PORT 0xE9   // QEMU/Bochs debug console

void debug_putc(char c)
{
#if defined(__GNUC__) || defined(__clang__)
    __asm__ __volatile__ ("outb %0, %1"
                          :
                          : "a"(c), "Nd"((uint16_t)DEBUG_PORT));
#elif defined(_MSC_VER)
    __outbyte(DEBUG_PORT, (unsigned char)c);
#else
    #error "debug_putc: no implementation for this compiler"
#endif
}

void debug(const char *str)
{
    while (*str)
        debug_putc(*str++);
}