#pragma once
#include <arch/noarch/interrupts/interrupts.hpp>
#include <stdint.h>
#include <packed.hpp>

// IDT can hold 256 entries (0-255)
#define IDT_MAX_ENTRIES 256

// IRQs are remapped to start at interrupt 32
#define IRQ_OFFSET 0x20

// Gate type flags
#define IDT_FLAG_GATE_INTERRUPT 0x8E // Present | Ring 0 | 32-bit interrupt gate
#define IDT_FLAG_GATE_TRAP 0x8F		 // Present | Ring 0 | 32-bit trap gate

// An IDT entry is 8 bytes, same structure as GDT but different fields:
//
//  Bits 0-15:  offset_low   — low 16 bits of handler address
//  Bits 16-31: selector     — code segment selector (GDT_CODE_SEGMENT = 0x08)
//  Bits 32-39: reserved     — always 0
//  Bits 40-47: flags        — type + DPL + present bit
//  Bits 48-63: offset_high  — high 16 bits of handler address
struct IDTEntry
{
	uint16_t offset_low;
	uint16_t selector;
	uint8_t reserved;
	uint8_t flags;
	uint16_t offset_high;
} PACKED;

// Same layout as GDTDescriptor — loaded with lidt
struct IDTDescriptor
{
	uint16_t limit;
	uint32_t base;
} PACKED;

// Struct pushed onto the stack by our asm stubs before calling the C++ handler.
// This is the "snapshot" of the CPU state when the interrupt fired.
struct InterruptFrame
{
	// Pushed by pusha
	uint32_t edi, esi, ebp, esp_dummy, ebx, edx, ecx, eax;
	// Pushed by our asm stub
	uint32_t interrupt_number;
	uint32_t error_code;
	// Pushed by CPU automatically
	uint32_t eip, cs, eflags;
} PACKED;



// Assembly function to load the IDT (like _load_gdt)
extern "C" void _load_idt(IDTDescriptor *idt);

// Initialize the IDT: set up all 256 entries, remap PIC, load IDT
void init_idt();

// The C++ handler that asm stubs call — declared extern "C" so asm can find it
extern "C" void isr_handler(InterruptFrame *frame);

// ISR stubs (defined in idt.asm) — one per interrupt 0-47
// These are declared so idt.cpp can take their addresses
extern "C"
{
	// CPU exceptions (0-31)
	extern void isr0();
	extern void isr1();
	extern void isr2();
	extern void isr3();
	extern void isr4();
	extern void isr5();
	extern void isr6();
	extern void isr7();
	extern void isr8();
	extern void isr9();
	extern void isr10();
	extern void isr11();
	extern void isr12();
	extern void isr13();
	extern void isr14();
	extern void isr15();
	extern void isr16();
	extern void isr17();
	extern void isr18();
	extern void isr19();
	extern void isr20();
	extern void isr21();
	extern void isr22();
	extern void isr23();
	extern void isr24();
	extern void isr25();
	extern void isr26();
	extern void isr27();
	extern void isr28();
	extern void isr29();
	extern void isr30();
	extern void isr31();
	// Hardware IRQs (32-47)
	extern void irq0();
	extern void irq1();
	extern void irq2();
	extern void irq3();
	extern void irq4();
	extern void irq5();
	extern void irq6();
	extern void irq7();
	extern void irq8();
	extern void irq9();
	extern void irq10();
	extern void irq11();
	extern void irq12();
	extern void irq13();
	extern void irq14();
	extern void irq15();
}

template <typename T>
void fill(T *arr, T val, int start, int end)
{
	for (int i = start; i < end; i++)
		arr[i] = val;
}

inline void (*isr[])() = {
	isr0, isr1, isr2, isr3, isr4, isr5, isr6, isr7,
	isr8, isr9, isr10, isr11, isr12, isr13, isr14, isr15,
	isr16, isr17, isr18, isr19, isr20, isr21, isr22, isr23,
	isr24, isr25, isr26, isr27, isr28, isr29, isr30, isr31};

inline void (*irqp[])() = {
	irq0, irq1, irq2, irq3, irq4, irq5, irq6, irq7,
	irq8, irq9, irq10, irq11, irq12, irq13, irq14, irq15};