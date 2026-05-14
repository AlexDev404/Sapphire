#pragma once
#include <arch/noarch/interrupts/interrupts.hpp>
#include <stdint.h>

namespace syscalls
{
    // Syscall setup: registers the syscall handler
    void init();
    // Send a syscall
    uint32_t send(uint32_t syscall_number, uint32_t arg1);
}
