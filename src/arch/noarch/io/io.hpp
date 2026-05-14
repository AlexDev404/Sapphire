#pragma once
#include <stdint.h>

namespace io
{
	// Read an 8-bit byte from a hardware register
	uint8_t read8(uintptr_t address);
	// Write an 8-bit byte to a hardware register
	void write8(uintptr_t address, uint8_t value);

	// Read a 16-bit word from a hardware register
	uint16_t read16(uintptr_t address);
	// Write a 16-bit word to a hardware register
	void write16(uintptr_t address, uint16_t value);

	// Read a 32-bit double-word from a hardware register
	uint32_t read32(uintptr_t address);
	// Write a 32-bit double-word to a hardware register
	void write32(uintptr_t address, uint32_t value);
}