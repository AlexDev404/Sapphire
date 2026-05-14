#include "write.hpp"
#include <driver/fb/kprint.hpp>

uint32_t sys_write(uint32_t arg1)
{
	const WriteArguments *args = reinterpret_cast<WriteArguments *>(arg1);
	uint32_t chars_processed = 0;
	while (chars_processed < args->count)
	{
		uint8_t c = reinterpret_cast<const char *>(args->buf)[chars_processed];
		kputc(c);
		chars_processed++;
	}
	return chars_processed;
}