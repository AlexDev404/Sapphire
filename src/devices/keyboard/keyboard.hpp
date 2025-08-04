#pragma once
#include <stdint.h>
#include <platform/x86.hpp>

// Keyboard data port
#define KEYBOARD_DATA_PORT    0x60
#define KEYBOARD_STATUS_PORT  0x64

uint8_t keyboard_read_scan_code(void);
char scancode_to_char(uint8_t scancode);
void keyboard_handle_scancode(uint8_t scancode);
