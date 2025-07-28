#pragma once

#ifdef __GNUC__
#define PACKED __attribute__((packed))
#else
#define PACKED
#endif

#define IDT_SIZE 256

struct PACKED idt_descriptor32
{
    unsigned short addr_low;  // Bits 0...15
    unsigned short selector;
    unsigned char always0;    // Unused field
    unsigned char flags;      // Gate Type, DPL, and p-fields
    unsigned short addr_high; // Bits 16..31
};

struct PACKED idt_ptr
{
    unsigned short limit;
    unsigned long base;
};

// Prototypes
void idt_set_gate(unsigned char vector, unsigned long *handler, unsigned short dpl);
void idt_init();