#pragma once
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


void disable_pic8259();