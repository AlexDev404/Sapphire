#pragma once
#include <stdint.h>

/**
 * @brief Read the value of CR3 control register.
 * @return The current value of CR3.
 */
extern "C" uint32_t _read_cr3(void);
/**
 * @brief Write a value to the CR3 control register.
 * @param value The value to write to CR3.
 */
extern "C" void _write_cr3(uint32_t value);
