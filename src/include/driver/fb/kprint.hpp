#pragma once
#include <driver/fb/primitives/string.hpp>
#include <driver/fb/primitives/char.hpp>
#include <stdint.h>
#include <stdarg.h>

// Print a null-terminated string at the current cursor position
void kprint(const char *str);
// Print a null-terminated string at a specific column and row (moves cursor there)
void kprint_at(const char *str, int col, int row);
// Print a single character at the current cursor position
void kputc(char c);
// Print a formatted string (supports %c, %d, %u, %x, %o, %s, %%)
void kprintf(const char *format, ...);
