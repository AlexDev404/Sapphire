#include <kernel/syscall.hpp>
#include <stdint.h>

// The inline asm constraint you'll need:
//   Output: EAX holds the return value
//   Inputs: EAX = number, EBX = arg1
//   The interrupt clobbers memory and possibly other registers
// Invoke a syscall via int 0x80.
uint32_t syscall(uint32_t number, uint32_t arg1)
{
	// Send off the syscall
	uint32_t result = syscalls::send(number, arg1);
	return result;
}
