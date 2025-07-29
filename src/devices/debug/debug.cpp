#include "debugcon.h"

// Only accepts characters from 0 - 9
char dbgp_int(int num)
{
    if (num > 9)
        return 0x0;
    return ('0' + num);
}

/*
 * Write some text to the debug port
*/
void send_debug(const char *str)
{
    // Save all registers before doing port I/O
    while (*str)
    {
#ifdef _MSC_VER
        _asm {
            push eax
            push edx
            mov al, byte ptr [str]
            mov dx, 0xE9
            out dx, al
            pop edx
            pop eax
        }
        str++;
#else
        // Tell GCC exactly which registers we're using and that we're clobbering them
        // asm volatile("outb %%al, %%dx" 
        //             : 
        //             : "a"(*str), "d"(0xE9)
        //             : "memory");
        interrupt_debug_char(*str);
        str++;
        #endif
    }
}


// Interrupt-safe debug output
void interrupt_debug_char(char c)
{
#ifndef _MSC_VER
    asm volatile(
        "pusha; "
        "movb %0, %%al; "
        "movw $0xE9, %%dx; "
        "outb %%al, %%dx; "
        "popa; "
        :
        : "r"(c)
        : "memory");
#endif
}
