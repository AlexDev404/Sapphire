#include "io_apic.hpp"
#include <devices/debug/debugcon.h>

// Global variable to store I/O APIC base address
static uint32_t* g_io_apic_base = nullptr;

// Extract IO APIC base address from MADT
uint32_t* get_io_apic_base() {
    if (g_io_apic_base != nullptr) {
        return g_io_apic_base;
    }
    
    // Get the MADT from the ACPI subsystem
    madt_t* madt = find_madt();
    if (!madt) {
        return nullptr; // MADT not found
    }
    
    // Iterate through the MADT entries to find the IO APIC entry
    uint8_t* entry_ptr = (uint8_t*)madt + sizeof(madt_t);
    uint8_t* madt_end = (uint8_t*)madt + madt->header.length;
    while (entry_ptr < madt_end) {
        madt_entry_header_t* entry = (madt_entry_header_t*)entry_ptr;
        if (entry->type == MADT_TYPE_IO_APIC) {
            madt_io_apic_t* io_apic = (madt_io_apic_t*)entry;
            // Cache and return the base address of the IO APIC
            g_io_apic_base = (uint32_t*)(uintptr_t)io_apic->io_apic_address;
            return g_io_apic_base;
        }
        entry_ptr += entry->length;
    }
    return nullptr; // IO APIC not found
}

// Read from I/O APIC register
uint32_t io_apic_read(uint32_t reg) {
    uint32_t* io_apic = get_io_apic_base();
    if (!io_apic) return 0;
    
    // Write register index to select register
    io_apic[IO_APIC_REG_SELECT / 4] = reg;
    // Read data from data register
    return io_apic[IO_APIC_REG_DATA / 4];
}

// Write to I/O APIC register
void io_apic_write(uint32_t reg, uint32_t value) {
    uint32_t* io_apic = get_io_apic_base();
    if (!io_apic) return;
    
    // Write register index to select register
    io_apic[IO_APIC_REG_SELECT / 4] = reg;
    // Write data to data register
    io_apic[IO_APIC_REG_DATA / 4] = value;
}

// Set up a redirection table entry for an IRQ
void io_apic_set_redirection_entry(uint8_t irq, uint8_t vector, uint8_t dest_apic_id) {
    // Calculate redirection table entry registers
    uint32_t low_reg = IO_APIC_REDTBL_BASE + (irq * 2);
    uint32_t high_reg = IO_APIC_REDTBL_BASE + (irq * 2) + 1;
    
    // Configure high 32 bits (destination)
    uint32_t high = ((uint32_t)dest_apic_id) << 24;
    io_apic_write(high_reg, high);
    
    // Configure low 32 bits (vector, delivery mode, etc.)
    uint32_t low = vector |                    // Interrupt vector
                   IO_APIC_DELIVERY_FIXED |    // Fixed delivery mode
                   IO_APIC_DEST_PHYSICAL |     // Physical destination mode
                   IO_APIC_POLARITY_HIGH |     // Active high polarity
                   IO_APIC_TRIGGER_EDGE |      // Edge triggered
                   IO_APIC_UNMASKED;           // Not masked (enabled)
    
    io_apic_write(low_reg, low);
}

// Mask an IRQ (disable it)
void io_apic_mask_irq(uint8_t irq) {
    uint32_t low_reg = IO_APIC_REDTBL_BASE + (irq * 2);
    uint32_t low = io_apic_read(low_reg);
    low |= IO_APIC_MASKED;  // Set mask bit
    io_apic_write(low_reg, low);
}

// Unmask an IRQ (enable it)
void io_apic_unmask_irq(uint8_t irq) {
    uint32_t low_reg = IO_APIC_REDTBL_BASE + (irq * 2);
    uint32_t low = io_apic_read(low_reg);
    low &= ~IO_APIC_MASKED;  // Clear mask bit
    io_apic_write(low_reg, low);
}

// Initialize I/O APIC
void io_apic_init() {
    uint32_t* io_apic = get_io_apic_base();
    if (!io_apic) {
        send_debug("I/O APIC: Not found in MADT\n");
        return;
    }
    
    send_debug("I/O APIC: Initializing at address 0x");
    
    // Print I/O APIC address in hex
    char hex_buffer[9];
    uint32_t addr = (uint32_t)(uintptr_t)io_apic;
    for (int i = 7; i >= 0; i--) {
        hex_buffer[i] = "0123456789ABCDEF"[addr & 0xF];
        addr >>= 4;
    }
    hex_buffer[8] = '\0';
    send_debug(hex_buffer);
    send_debug("\n");
    
    // Read I/O APIC version to get number of supported interrupts
    uint32_t version = io_apic_read(IO_APIC_VERSION);
    uint8_t max_redirection_entries = ((version >> 16) & 0xFF) + 1;
    
    send_debug("I/O APIC: Maximum redirection entries: ");
    char num_str[4];
    num_str[0] = '0' + (max_redirection_entries / 10);
    num_str[1] = '0' + (max_redirection_entries % 10);
    num_str[2] = '\n';
    num_str[3] = '\0';
    send_debug(num_str);
    
    // Mask all interrupts initially
    for (uint8_t i = 0; i < max_redirection_entries; i++) {
        io_apic_mask_irq(i);
    }
    
    send_debug("I/O APIC: Initialization complete\n");
}

// Set up keyboard interrupt routing through I/O APIC
void io_apic_setup_keyboard_interrupt(uint8_t vector, uint8_t dest_apic_id) {
    send_debug("I/O APIC: Setting up keyboard interrupt (IRQ1)\n");
    
    // Set up redirection entry for IRQ1 (keyboard)
    io_apic_set_redirection_entry(IRQ_KEYBOARD, vector, dest_apic_id);
    
    send_debug("I/O APIC: Keyboard interrupt configured\n");
    send_debug("I/O APIC: - IRQ: 1\n");
    send_debug("I/O APIC: - Vector: 0x");
    
    // Print vector in hex
    char hex_str[3];
    hex_str[0] = "0123456789ABCDEF"[(vector >> 4) & 0xF];
    hex_str[1] = "0123456789ABCDEF"[vector & 0xF];
    hex_str[2] = '\0';
    send_debug(hex_str);
    send_debug("\n");
    
    send_debug("I/O APIC: - Destination APIC ID: ");
    char id_str[3];
    id_str[0] = '0' + (dest_apic_id / 10);
    id_str[1] = '0' + (dest_apic_id % 10);
    id_str[2] = '\0';
    send_debug(id_str);
    send_debug("\n");
}
