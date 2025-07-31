#pragma once
#include "../../include/platform/x86.hpp"

/*
 * APIC (Advanced Programmable Interrupt Controller) header
 * This file contains definitions and functions related to the APIC
 * for handling interrupts in a modern x86 architecture.
 */

#define PIC_COMMAND_MASTER 0x20
#define PIC_COMMAND_SLAVE 0xA0
#define PIC_DATA_MASTER 0x21
#define PIC_DATA_SLAVE 0xA1
#define ICW_1 0x11 // Initialization Command Word 1
#define ICW_2_MASTER 0x20 // Vector offset for master PIC
#define ICW_2_SLAVE 0x28 // Vector offset for slave PIC
#define ICW_3_MASTER 0x02 // Master PIC has a slave on IRQ2
#define ICW_3_SLAVE 0x04 // Slave PIC is on IRQ2
#define ICW_4 0x01 // 8086/88 mode, no special features


void disable_pic8259();