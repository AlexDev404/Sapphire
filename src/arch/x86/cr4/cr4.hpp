#pragma once
#include <stdint.h>

/**
 * @brief Read the value of CR4 control register.
 * @return The current value of CR4.
 */
extern "C" uint32_t _read_cr4(void);
/**
 * @brief Write a value to the CR4 control register.
 * @param value The value to write to CR4.
 */
extern "C" void _write_cr4(uint32_t value);
