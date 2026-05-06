#pragma once
#include <stdint.h>

/**
 * @brief Initialize platform-specific devices
 * @note Internal use only.
 */
extern "C" void platform_init(uint32_t boot_info_ptr);

void platform_interrupts_enable();
void platform_interrupts_disable();
