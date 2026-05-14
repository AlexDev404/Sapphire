#include "io.hpp"
#include <arch/noarch/io/io.hpp>
#include <arch/x86/pic/pic.hpp>

// Read an 8-bit byte to a hardware register
uint8_t io::read8(uintptr_t address)
{
	return inb(address);
}
// Write an 8-bit byte to a hardware register
void io::write8(uintptr_t address, uint8_t value)
{
	outb(address, value);
	return;
}

// Read a 16-bit word from a hardware register
uint16_t io::read16(uintptr_t address)
{
	return inw(address);
}
// Write a 16-bit word to a hardware register
void io::write16(uintptr_t address, uint16_t value)
{
	outw(address, value);
	return;
}
// Read a 32-bit double-word from a hardware register
uint32_t io::read32(uintptr_t address)
{
	return inl(address);
}
// Write a 32-bit double-word to a hardware register
void io::write32(uintptr_t address, uint32_t value)
{
	outl(address, value);
	return;
}
