#pragma once
#include <stdint.h>
#include <packed.hpp>

struct E820Entry {
    uint64_t base;
    uint64_t length;
    uint32_t type;
    // uint32_t acpi_attrs;
} PACKED;

struct BootInfo {
    E820Entry* e820_map;         // offset +0 — matches boot_info.e820_map in stage2.asm
    uint32_t e820_entry_count;   // offset +4 — matches boot_info.e820_entry_count in stage2.asm
    uint32_t* page_directory;    // offset +8 — matches boot_info.page_directory in stage2.asm
} PACKED;