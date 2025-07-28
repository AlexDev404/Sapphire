#pragma once

char dbgp_int(int num);             // Debug parse int to char
void send_debug(const char *str);   // Debug send to console
void interrupt_debug_char(char c);  // Interrupt-context safe debug output