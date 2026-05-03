#pragma once

/**
 * @brief Write a character to the debug port
 * @param c The character to write to the debug port
 */
void debug_putc(char c);
/**
 * @brief Write some text to the debug port
 * @param str The string to write to the debug port
 */
void debug(const char *str);