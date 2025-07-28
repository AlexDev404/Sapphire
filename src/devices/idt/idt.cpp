#include "idt.hpp"

struct idt_descriptor32 idt[IDT_SIZE];

void idt_set_gate(unsigned char vector, unsigned long *handler, unsigned short dpl)
{
    unsigned long handler_addr = (unsigned long)handler;
    struct idt_descriptor32 *entry = &idt[vector];
    entry->addr_high = (handler_addr >> 16) & 0xFFFF;
    entry->addr_low = handler_addr & 0xFFFF;
    entry->selector = 0x08;                                 // Kernel select (from: GDT)
    entry->always0 = 0;                                     // IST
    entry->flags = 0b1110 | ((dpl & 0b11) << 5) | (1 << 7); // Interrupt gate (present)
}

void idt_init()
{
    struct idt_ptr idt_p;
    idt_p.limit = (sizeof(struct idt_descriptor32) * IDT_SIZE) - 1;
    idt_p.base = (unsigned long)idt;
#ifdef _MSC_VER
    _asm {
        lidt idt_p
    }
#else
    asm volatile("lidt %0" : : "m"(idt_p));
#endif
}