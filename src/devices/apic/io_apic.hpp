#pragma once
#include <stdint.h>
#include <devices/acpi/acpi.hpp>

// I/O APIC register offsets
#define IO_APIC_REG_SELECT 0x00  // Register select (index)
#define IO_APIC_REG_DATA   0x10  // Register data (window)

// I/O APIC registers (accessed via index/data)
#define IO_APIC_ID         0x00  // I/O APIC ID
#define IO_APIC_VERSION    0x01  // I/O APIC Version
#define IO_APIC_ARB        0x02  // I/O APIC Arbitration
#define IO_APIC_REDTBL_BASE 0x10 // Redirection Table base

// Redirection Table Entry flags
#define IO_APIC_DELIVERY_FIXED    (0 << 8)
#define IO_APIC_DELIVERY_LOWEST   (1 << 8)
#define IO_APIC_DELIVERY_SMI      (2 << 8)
#define IO_APIC_DELIVERY_NMI      (4 << 8)
#define IO_APIC_DELIVERY_INIT     (5 << 8)
#define IO_APIC_DELIVERY_EXTINT   (7 << 8)

#define IO_APIC_DEST_PHYSICAL     (0 << 11)
#define IO_APIC_DEST_LOGICAL      (1 << 11)

#define IO_APIC_POLARITY_HIGH     (0 << 13)
#define IO_APIC_POLARITY_LOW      (1 << 13)

#define IO_APIC_TRIGGER_EDGE      (0 << 15)
#define IO_APIC_TRIGGER_LEVEL     (1 << 15)

#define IO_APIC_MASKED            (1 << 16)
#define IO_APIC_UNMASKED          (0 << 16)

// IRQ to I/O APIC pin mapping
#define IRQ_TIMER    0  // Maps to I/O APIC pin 2 (special case)
#define IRQ_KEYBOARD 1  // Maps to I/O APIC pin 1

// Function prototypes
uint32_t* get_io_apic_base();
void io_apic_init();
uint32_t io_apic_read(uint32_t reg);
void io_apic_write(uint32_t reg, uint32_t value);
void io_apic_set_redirection_entry(uint8_t irq, uint8_t vector, uint8_t dest_apic_id);
void io_apic_mask_irq(uint8_t irq);
void io_apic_unmask_irq(uint8_t irq);
void io_apic_setup_keyboard_interrupt(uint8_t vector, uint8_t dest_apic_id);