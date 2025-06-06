#include "debug.h"


void send_debug(const char* str) {
    // In a real system, this would send the string to a debug console or log
    // For now, we will just print it to the standard output
    // This is a placeholder implementation
    while (*str) {
        #ifdef _MSC_VER
            _asm {
                mov al, byte ptr [str]
                mov dx, 0xE9
                out dx, al
            }
            str++;
        #else
            asm volatile ("outb %%al, %%dx" : : "a" (*str++), "d" (0xE9));
        #endif
    }
}