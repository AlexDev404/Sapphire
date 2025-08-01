#pragma once

char dbgp_int(int num);             // Debug parse int to char
extern "C" void send_debug(const char *str);   // Debug send to console
extern "C" void interrupt_debug_char(char c);  // Interrupt-context safe debug output