#include "syscall.hpp"
static syscall_imp syscall_table[256];
static void syscall_handler(InterruptFrame *frame);

void syscalls::init()
{
	// Register syscall_handler for interrupt 0x80 (128).
	interrupts::register_handler(0x80, syscall_handler);
	fill(syscall_table, sys_default, 0, 256);
	syscall_table[0] = sys_read;
	syscall_table[1] = sys_write;
}

uint32_t syscalls::send(uint32_t syscall_number, uint32_t arg1)
{
	return send_syscall(syscall_number, arg1);
	
}

// The syscall dispatcher — called when int 0x80 fires.
// The syscall number is in frame->eax, first arg in frame->ebx.
// Put the return value back in frame->eax before returning.
static void syscall_handler(InterruptFrame *frame)
{
	// Read the syscall number from frame->eax.
	uint32_t syscall_number = frame->eax & 0xFF;
	// Switch on the syscall number and handle each case:

	//
	//   0: SYS_READ:
	//     Read a character from the keyboard.
	//     Think about what happens if no input is available yet.
	//     Store the result back into frame->eax.
	//
	//   1: SYS_WRITE:
	//     The character to write is in frame->ebx.
	//     Print it to screen.
	//     No meaningful return value needed.
	//
	//   default:
	//     Unknown syscall — set frame->eax to -1 or similar.
	frame->eax = syscall_table[syscall_number](frame->ebx);
	return;
}

uint32_t sys_default(uint32_t)
{
	return -1;
}
