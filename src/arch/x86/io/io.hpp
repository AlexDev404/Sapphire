#pragma once
#include <stdint.h>

// Port I/O helpers

// Byte
extern "C" uint8_t inb(uint16_t port);
extern "C" void outb(uint16_t port, uint8_t value);

// Word!!
extern "C" uint16_t inw(uint16_t port);
extern "C" void outw(uint16_t port, uint16_t value);

// Long
extern "C" uint32_t inl(uint16_t port);
extern "C" void outl(uint16_t port, uint32_t value);