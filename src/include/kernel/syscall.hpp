#pragma once
#include <stdint.h>
#include <arch/noarch/syscall/syscall.hpp>

// Syscall numbers
#define SYS_READ  0   // Read a character from keyboard
#define SYS_WRITE 1   // Write a character to screen

// Invoke a syscall from kernel/user code.
// number = syscall number (SYS_READ, SYS_WRITE, etc.)
// arg1   = first argument (meaning depends on syscall)
//
// Returns: result value (meaning depends on syscall)
//
// On x86, this triggers int 0x80 with:
//   EAX = syscall number
//   EBX = arg1
//   Return value comes back in EAX
uint32_t syscall(uint32_t number, uint32_t arg1 = 0);

struct WriteArgs {
    uint32_t count;
    uint32_t fd;
    void* buf;
};
struct ReadArgs {
    uint32_t count;
    uint32_t fd;
    void* buf;
};