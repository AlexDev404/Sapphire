#include "strings.hpp"

const char *hex2str(unsigned char value)
{
	static char buffer[3];
	static char hex_digits[] = "0123456789ABCDEF";
	buffer[0] = hex_digits[value >> 4];
	buffer[1] = hex_digits[value & 0x0F];
	buffer[2] = '\0';
	return buffer;
}
