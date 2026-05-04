#pragma once
#include <driver/fb/primitives/string.hpp>
#include <driver/fb/primitives/char.hpp>

// Print a null-terminated string to the screen
void kprint(const char *str);
// Print a null-terminated string to the screen at a specific column and row
void kprint_at(const char *str, int col, int row);
// Print a single character to the screen
void kputc(char c);