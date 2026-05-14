#pragma once
#include <stdint.h>

// Platform-agnostic interrupt interface.
// Each architecture defines its own InterruptFrame layout and includes this header
// to provide a common API for registering handlers.

// Interrupt frame
struct InterruptFrame;

// Function pointer type for interrupt handlers
typedef void (*isr_t)(InterruptFrame *frame);

// Register a handler for a specific interrupt number.
void register_interrupt_handler(uint8_t intno, isr_t handler);

// Enable/disable interrupts (platform-specific implementation)
void interrupts_enable();
void interrupts_disable();
