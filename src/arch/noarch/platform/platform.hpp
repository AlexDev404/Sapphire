#pragma once

/**
 * @brief Initialize platform-specific devices
 * @note Internal use only.
 */
extern "C" void platform_init();

void platform_interrupts_enable();
void platform_interrupts_disable();
