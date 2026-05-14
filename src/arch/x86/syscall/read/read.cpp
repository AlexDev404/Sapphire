#include "read.hpp"
#include <driver/keyboard/keyboard.hpp>

uint32_t sys_read(uint32_t arg1)
{
	ReadArguments *args = reinterpret_cast<ReadArguments *>(arg1);
	uint32_t chars_processed = 0;
	while (chars_processed < args->count)
	{
		if (!keyboard::has_input())
			return chars_processed;
		// Write into the buffer
		reinterpret_cast<char *>(args->buf)[chars_processed] = keyboard::read_char();
		chars_processed++;
	}
	return chars_processed;
}