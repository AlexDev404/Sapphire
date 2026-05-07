#pragma once
#include <stdint.h>

/**
 * @brief Read the value of CR0 control register.
 * @return The current value of CR0.
 */
extern "C" uint32_t _read_cr0(void);
/**
 * @brief Write a value to the CR0 control register.
 * @param value The value to write to CR0.
 */
extern "C" void _write_cr0(uint32_t value);
