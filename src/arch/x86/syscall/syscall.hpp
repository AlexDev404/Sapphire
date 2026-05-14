#pragma once
#include <arch/noarch/syscall/syscall.hpp>
#include "read/read.hpp"
#include "write/write.hpp"
#include <arch/x86/idt/idt.hpp>
#include <kernel/syscall.hpp>
#include <driver/keyboard/keyboard.hpp>
#include <driver/fb/kprint.hpp>
#include <fill.hpp>

typedef uint32_t (*syscall_imp)(uint32_t);
extern "C" uint32_t send_syscall(uint32_t syscall_number, uint32_t arg1);
uint32_t sys_default(uint32_t);