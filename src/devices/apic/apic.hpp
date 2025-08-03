#pragma once
#include <stdint.h>
#include <platform/x86.hpp>

/*
 * APIC (Advanced Programmable Interrupt Controller) header
 * This file contains definitions and functions related to the APIC
 * for handling interrupts in a modern x86 architecture.
 */

// Ports for the Programmable Interrupt Controllers (PICs)
// These are used to control the legacy PICs in x86 systems.
#define PIC_COMMAND_MASTER 0x20
#define PIC_COMMAND_SLAVE 0xA0

// Data ports for the PICs
#define PIC_DATA_MASTER 0x21
#define PIC_DATA_SLAVE 0xA1

// Initialization Command Words (ICWs) for the PICs
#define ICW_1 0x11 // Initialization Command Word 1

// Vector offsets for the master and slave PICs
// These define the starting interrupt vector numbers for the master and slave PICs.
#define ICW_2_MASTER 0x20 // Vector offset for master PIC
#define ICW_2_SLAVE 0x28 // Vector offset for slave PIC

// Additional ICWs for the PICs
// These define the cascade identity and mode of operation.
#define ICW_3_MASTER 0x02 // Master PIC has a slave on IRQ2
#define ICW_3_SLAVE 0x04 // Slave PIC is on IRQ2

// ICW 4 defines the mode of operation for the PICs
#define ICW_4 0x01 // 8086/88 mode, no special features

// APIC MSR and register definitions
#define IA32_APIC_BASE_MSR 0x1B // MSR for APIC base address
#define IA32_APIC_BASE_ENABLE (1ULL << 11) // Bit to enable the APIC
#define IA32_APIC_BASE_BSP (1ULL << 8) // Bootstrap processor flag

// LAPIC register offsets
#define IA32_LAPIC_ID_OFFSET 0x20 // LAPIC ID offset
#define IA32_LAPIC_VERSION_OFFSET 0x30 // LAPIC version offset
#define IA32_LAPIC_TPR_OFFSET 0x80 // Task Priority Register
#define IA32_LAPIC_APR_OFFSET 0x90 // Arbitration Priority Register
#define IA32_LAPIC_PPR_OFFSET 0xA0 // Processor Priority Register
#define IA32_LAPIC_EOI_OFFSET 0xB0 // End of Interrupt (EOI) offset
#define IA32_LAPIC_RRD_OFFSET 0xC0 // Remote Read Register
#define IA32_LAPIC_LDR_OFFSET 0xD0 // Logical Destination Register
#define IA32_LAPIC_DFR_OFFSET 0xE0 // Destination Format Register
#define IA32_LAPIC_SPURIOUS_OFFSET 0xF0 // Spurious interrupt vector offset
#define IA32_LAPIC_ISR_OFFSET 0x100 // In-Service Register (base)
#define IA32_LAPIC_TMR_OFFSET 0x180 // Trigger Mode Register (base)
#define IA32_LAPIC_IRR_OFFSET 0x200 // Interrupt Request Register (base)
#define IA32_LAPIC_ESR_OFFSET 0x280 // Error Status Register
#define IA32_LAPIC_ICR_LOW_OFFSET 0x300 // Interrupt Command Register (low)
#define IA32_LAPIC_ICR_HIGH_OFFSET 0x310 // Interrupt Command Register (high)
#define IA32_LAPIC_TIMER_LVT_OFFSET 0x320 // Local Vector Table (LVT) timer offset
#define IA32_LAPIC_THERMAL_LVT_OFFSET 0x330 // LVT thermal sensor
#define IA32_LAPIC_PERF_LVT_OFFSET 0x340 // LVT performance monitoring
#define IA32_LAPIC_LINT0_LVT_OFFSET 0x350 // LVT LINT0
#define IA32_LAPIC_LINT1_LVT_OFFSET 0x360 // LVT LINT1
#define IA32_LAPIC_ERROR_LVT_OFFSET 0x370 // LVT error
#define IA32_LAPIC_TIMER_INITIAL_COUNT_OFFSET 0x380 // Timer initial count
#define IA32_LAPIC_TIMER_CURRENT_COUNT_OFFSET 0x390 // Timer current count
#define IA32_LAPIC_TIMER_DIVIDE_CONFIG_OFFSET 0x3E0 // Timer divide configuration

// LAPIC timer divide values
#define LAPIC_TIMER_DIVIDE_BY_2 0x0
#define LAPIC_TIMER_DIVIDE_BY_4 0x1
#define LAPIC_TIMER_DIVIDE_BY_8 0x2
#define LAPIC_TIMER_DIVIDE_BY_16 0x3
#define LAPIC_TIMER_DIVIDE_BY_32 0x8
#define LAPIC_TIMER_DIVIDE_BY_64 0x9
#define LAPIC_TIMER_DIVIDE_BY_128 0xA
#define LAPIC_TIMER_DIVIDE_BY_1 0xB

// LAPIC timer modes
#define LAPIC_TIMER_ONE_SHOT 0x0
#define LAPIC_TIMER_PERIODIC 0x1
#define LAPIC_TIMER_TSC_DEADLINE 0x2

// LVT flags
#define LAPIC_LVT_MASKED (1 << 16)
#define LAPIC_LVT_TRIGGER_LEVEL (1 << 15)
#define LAPIC_LVT_TRIGGER_EDGE (0 << 15)
#define LAPIC_LVT_POLARITY_LOW (1 << 13)
#define LAPIC_LVT_POLARITY_HIGH (0 << 13)
#define LAPIC_LVT_DELIVERY_FIXED (0 << 8)
#define LAPIC_LVT_DELIVERY_NMI (4 << 8)
#define LAPIC_LVT_DELIVERY_INIT (5 << 8)
#define LAPIC_LVT_DELIVERY_EXTINT (7 << 8)

// Function prototypes
void disable_pic8259();
void enable_lapic();
void lapic_send_eoi();
uint32_t lapic_get_id();
void lapic_timer_setup(uint32_t vector, uint32_t initial_count, uint32_t divide_config, uint32_t mode);
void lapic_set_timer_initial_count(uint32_t count);
uint32_t lapic_get_timer_current_count();
void lapic_disable_timer();
uint32_t* get_lapic_base();