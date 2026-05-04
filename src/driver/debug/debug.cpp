#include <driver/debug/debug.hpp>
#include <arch/noarch/debug/debug.hpp>

void kdebug(const char *str) {
	debug("[kernel]: ");
	debug(str);
	debug("\n");
}
void kdebug_putc(char c) {
	debug_putc(c);
}
