#include "acpi.hpp"
#include <devices/debug/debugcon.h>
#include <graphics/print_text.hpp>

// Type definition for size_t
typedef unsigned int size_t;

// Global variables to store ACPI information
static rsdp_t* g_rsdp = nullptr;
static madt_t* g_madt = nullptr;

// Memory comparison function
static int memcmp(const void* s1, const void* s2, size_t n) {
    const unsigned char* p1 = (const unsigned char*)s1;
    const unsigned char* p2 = (const unsigned char*)s2;
    for (size_t i = 0; i < n; i++) {
        if (p1[i] != p2[i]) {
            return p1[i] - p2[i];
        }
    }
    return 0;
}

rsdp_t* find_rsdp() {
    // Search for RSDP in EBDA (Extended BIOS Data Area)
    uint16_t* ebda_address = (uint16_t*)0x40E;
    uint32_t ebda = 0;
    
    // Safely read EBDA address if valid
    if (ebda_address != nullptr) {
        ebda = (*ebda_address) << 4;
    }
    
    // Search EBDA (typically 0x80000 to 0x9FFFF) only if valid
    if (ebda >= 0x80000 && ebda < 0xA0000) {
        for (uint32_t addr = ebda; addr < ebda + 1024; addr += 16) {
            rsdp_t* rsdp = (rsdp_t*)addr;
            if (rsdp && memcmp(rsdp->signature, "RSD PTR ", 8) == 0) {
                return rsdp;
            }
        }
    }
    
    // Search BIOS ROM area (0xE0000 to 0xFFFFF)
    for (uint32_t addr = 0xE0000; addr < 0x100000; addr += 16) {
        rsdp_t* rsdp = (rsdp_t*)addr;
        if (memcmp(rsdp->signature, "RSD PTR ", 8) == 0) {
            return rsdp;
        }
    }
    
    return nullptr;
}

bool validate_rsdp(rsdp_t* rsdp) {
    if (!rsdp) return false;
    
    // Validate ACPI 1.0 checksum (first 20 bytes)
    uint8_t checksum = 0;
    uint8_t* bytes = (uint8_t*)rsdp;
    for (int i = 0; i < 20; i++) {
        checksum += bytes[i];
    }
    
    if (checksum != 0) {
        return false;
    }
    
    // If ACPI 2.0+, validate extended checksum
    if (rsdp->revision >= 2) {
        checksum = 0;
        for (uint32_t i = 0; i < rsdp->length; i++) {
            checksum += bytes[i];
        }
        if (checksum != 0) {
            return false;
        }
    }
    
    return true;
}

bool validate_sdt_header(sdt_header_t* header) {
    if (!header) return false;
    
    // Validate checksum
    uint8_t checksum = 0;
    uint8_t* bytes = (uint8_t*)header;
    for (uint32_t i = 0; i < header->length; i++) {
        checksum += bytes[i];
    }
    
    return checksum == 0;
}

void parse_rsdt(rsdt_t* rsdt) {
    if (!rsdt) return;
    
    send_debug("ACPI: Parsing RSDT\n");
    
    // Calculate number of SDT entries
    uint32_t entries = (rsdt->header.length - sizeof(sdt_header_t)) / 4;
    
    for (uint32_t i = 0; i < entries; i++) {
        sdt_header_t* sdt = (sdt_header_t*)rsdt->sdt_pointers[i];
        
        if (validate_sdt_header(sdt)) {
            // Check if this is the MADT
            if (memcmp(sdt->signature, "APIC", 4) == 0) {
                g_madt = (madt_t*)sdt;
                send_debug("ACPI: Found MADT (APIC table)\n");
                parse_madt(g_madt);
            }
        }
    }
}

void parse_xsdt(xsdt_t* xsdt) {
    if (!xsdt) return;
    
    send_debug("ACPI: Parsing XSDT\n");
    
    // Calculate number of SDT entries
    uint32_t entries = (xsdt->header.length - sizeof(sdt_header_t)) / 8;
    
    for (uint32_t i = 0; i < entries; i++) {
        sdt_header_t* sdt = (sdt_header_t*)xsdt->sdt_pointers[i];
        
        if (validate_sdt_header(sdt)) {
            // Check if this is the MADT
            if (memcmp(sdt->signature, "APIC", 4) == 0) {
                g_madt = (madt_t*)sdt;
                send_debug("ACPI: Found MADT (APIC table)\n");
                parse_madt(g_madt);
            }
        }
    }
}

void parse_madt(madt_t* madt) {
    if (!madt) return;
    
    send_debug("ACPI: Parsing MADT\n");
    send_debug("ACPI: Local APIC Address: 0x");
    
    // Print LAPIC address in hex
    char hex_buffer[9];
    uint32_t addr = madt->local_apic_address;
    for (int i = 7; i >= 0; i--) {
        hex_buffer[i] = "0123456789ABCDEF"[addr & 0xF];
        addr >>= 4;
    }
    hex_buffer[8] = '\0';
    send_debug(hex_buffer);
    send_debug("\n");
    
    // Parse MADT entries
    uint8_t* entry_ptr = (uint8_t*)madt + sizeof(madt_t);
    uint8_t* madt_end = (uint8_t*)madt + madt->header.length;
    
    while (entry_ptr < madt_end) {
        madt_entry_header_t* entry = (madt_entry_header_t*)entry_ptr;
        
        switch (entry->type) {
            case MADT_TYPE_LOCAL_APIC: {
                madt_local_apic_t* lapic = (madt_local_apic_t*)entry;
                send_debug("ACPI: Found Local APIC - ID: ");
                char id_str[4];
                id_str[0] = '0' + (lapic->apic_id / 10);
                id_str[1] = '0' + (lapic->apic_id % 10);
                id_str[2] = '\n';
                id_str[3] = '\0';
                send_debug(id_str);
                break;
            }
            case MADT_TYPE_IO_APIC: {
                madt_io_apic_t* io_apic = (madt_io_apic_t*)entry;
                send_debug("ACPI: Found I/O APIC - Address: 0x");
                
                // Print I/O APIC address in hex
                uint32_t io_addr = io_apic->io_apic_address;
                for (int i = 7; i >= 0; i--) {
                    hex_buffer[i] = "0123456789ABCDEF"[io_addr & 0xF];
                    io_addr >>= 4;
                }
                send_debug(hex_buffer);
                send_debug("\n");
                break;
            }
            case MADT_TYPE_INTERRUPT_OVERRIDE:
                send_debug("ACPI: Found Interrupt Source Override\n");
                break;
        }
        
        entry_ptr += entry->length;
    }
}

madt_t* find_madt() {
    return g_madt;
}

void acpi_init()
{
    send_debug("ACPI: Initializing ACPI subsystem\n");
    
    // Find RSDP (Root System Description Pointer)
    rsdp_t* rsdp = find_rsdp();
    if (!rsdp) {
        print_text("ACPI: RSDP not found", COLOR_RED, COLOR_BLACK, 30, 180);
        send_debug("ACPI: RSDP not found\n");
        return;
    }
    
    g_rsdp = rsdp;
    send_debug("ACPI: RSDP found\n");
    
    // Validate RSDP checksum
    if (!validate_rsdp(rsdp)) {
        print_text("ACPI: Invalid RSDP", COLOR_RED, COLOR_BLACK, 30, 180);
        send_debug("ACPI: Invalid RSDP checksum\n");
        return;
    }
    
    send_debug("ACPI: RSDP validated\n");
    
    // Get RSDT/XSDT
    if (rsdp->revision >= 2) {
        // ACPI 2.0+ - use XSDT
        send_debug("ACPI: Using XSDT (ACPI 2.0+)\n");
        xsdt_t* xsdt = (xsdt_t*)rsdp->xsdt_address;
        if (validate_sdt_header(&xsdt->header)) {
            parse_xsdt(xsdt);
        } else {
            send_debug("ACPI: Invalid XSDT\n");
        }
    } else {
        // ACPI 1.0 - use RSDT
        send_debug("ACPI: Using RSDT (ACPI 1.0)\n");
        rsdt_t* rsdt = (rsdt_t*)rsdp->rsdt_address;
        if (validate_sdt_header(&rsdt->header)) {
            parse_rsdt(rsdt);
        } else {
            send_debug("ACPI: Invalid RSDT\n");
        }
    }
    
    print_text("ACPI Initialized", COLOR_GREEN, COLOR_BLACK, 30, 180);
    send_debug("ACPI: Initialization complete\n");
}