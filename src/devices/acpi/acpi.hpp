#pragma once
#include <stdint.h>

#ifdef __GNUC__
#define PACKED __attribute__((packed))
#else
#define PACKED
#endif

// ACPI System Description Table Header
struct PACKED sdt_header_t
{
    char signature[4];
    uint32_t length;
    uint8_t revision;
    uint8_t checksum;
    char oem_id[6];
    char oem_table_id[8];
    uint32_t oem_revision;
    uint32_t creator_id;
    uint32_t creator_revision;
};

// Root System Description Pointer
struct PACKED rsdp_t
{
    char signature[8];
    uint8_t checksum;
    char oem_id[6];
    uint8_t revision;
    uint32_t rsdt_address;
    // ACPI 2.0+ fields (only present if revision >= 2)
    uint32_t length;
    uint64_t xsdt_address;
    uint8_t extended_checksum;
    uint8_t reserved[3];
};

// Root System Description Table (ACPI 1.0)
struct PACKED rsdt_t
{
    sdt_header_t header;
    uint32_t sdt_pointers[];  // Variable length array
};

// Extended System Description Table (ACPI 2.0+)
struct PACKED xsdt_t
{
    sdt_header_t header;
    uint64_t sdt_pointers[];  // Variable length array
};

// Multiple APIC Description Table
struct PACKED madt_t
{
    sdt_header_t header;
    uint32_t local_apic_address;
    uint32_t flags;
    // Variable length MADT entries follow
};

// MADT Entry Header
struct PACKED madt_entry_header_t
{
    uint8_t type;
    uint8_t length;
};

// MADT Entry Types
#define MADT_TYPE_LOCAL_APIC 0
#define MADT_TYPE_IO_APIC 1
#define MADT_TYPE_INTERRUPT_OVERRIDE 2
#define MADT_TYPE_NMI_SOURCE 3
#define MADT_TYPE_LOCAL_APIC_NMI 4

// I/O APIC MADT Entry
struct PACKED madt_io_apic_t
{
    madt_entry_header_t header;
    uint8_t io_apic_id;
    uint8_t reserved;
    uint32_t io_apic_address;
    uint32_t global_system_interrupt_base;
};

// Local APIC MADT Entry
struct PACKED madt_local_apic_t
{
    madt_entry_header_t header;
    uint8_t processor_id;
    uint8_t apic_id;
    uint32_t flags;
};

// Function prototypes
void acpi_init();
rsdp_t* find_rsdp();
bool validate_rsdp(rsdp_t* rsdp);
bool validate_sdt_header(sdt_header_t* header);
void parse_rsdt(rsdt_t* rsdt);
void parse_xsdt(xsdt_t* xsdt);
void parse_madt(madt_t* madt);
madt_t* find_madt();
