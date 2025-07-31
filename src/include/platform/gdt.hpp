#pragma once
#include <stdint.h>

#ifdef __GNUC__
#define PACKED __attribute__((packed))
#define CDECL __attribute__((cdecl))
#else
#define PACKED
#define CDECL
#endif

struct PACKED GDTEntry
{
    uint16_t limit_low;
    uint16_t base_low;
    uint8_t base_middle;
    uint8_t access;
    uint8_t granularity;
    uint8_t base_high;
};

struct PACKED GDTDescriptor
{
    uint16_t limit;
    GDTEntry *base_ptr;
};

enum GDT_ACCESS
{
    GDT_ACCESS_CODE_READABLE = 0x02,
    GDT_ACCESS_DATA_WRITEABLE = 0x02,

    GDT_ACCESS_CODE_CONFORMING = 0x04,
    GDT_ACCESS_DATA_DIRECTION_NORMAL = 0x00,
    GDT_ACCESS_DATA_DIRECTION_DOWN = 0x04,

    GDT_ACCESS_DATA_SEGMENT = 0x10,
    GDT_ACCESS_CODE_SEGMENT = 0x18,

    GDT_ACCESS_DESCRIPTOR_TSS = 0x00,

    GDT_ACCESS_RING0 = 0x00,
    GDT_ACCESS_RING1 = 0x20,
    GDT_ACCESS_RING2 = 0x40,
    GDT_ACCESS_RING3 = 0x60,

    GDT_ACCESS_PRESENT = 0x80
};

enum GDT_FLAGS
{
    GDT_FLAG_64BIT = 0x20,
    GDT_FLAG_32BIT = 0x40,
    GDT_FLAG_16BIT = 0x00,

    GDT_FLAG_GRANULARITY_4K = 0x80,
    GDT_FLAG_GRANULARITY_1B = 0x00
};

#define GDT_LIMIT_LOW(limit) ((limit) & 0xFFFF)

#define GDT_BASE_LOW(base) ((base) & 0xFFFF)
#define GDT_BASE_MIDDLE(base) (((base) >> 16) & 0xFF)

#define GDT_FLAGS_LIMIT_HI(limit, flags) ((((limit) >> 16) & 0x0F) | ((flags) & 0xF0))
#define GDT_BASE_HIGH(base) (((base) >> 24) & 0xFF)

#define GDT_ENTRY(base, limit, access, flags) { \
    GDT_LIMIT_LOW(limit),                       \
    GDT_BASE_LOW(base),                         \
    GDT_BASE_MIDDLE(base),                      \
    (access),                                   \
    GDT_FLAGS_LIMIT_HI(limit, flags),           \
    GDT_BASE_HIGH(base)}

#define GDT_CODE_SEGMENT 0x08
#define GDT_DATA_SEGMENT 0x10

extern "C" void CDECL g_GDTLoad(GDTDescriptor *gdt, uint16_t code_segment_selector, uint16_t data_segment_selector);
void g_GDTInit();
