#include "apic.hpp"

void disable_pic8259()
{
    // Disable the master PIC
    outportb(PIC_COMMAND_MASTER, ICW_1); // Initialize command for master PIC
    outportb(PIC_DATA_MASTER, ICW_2_MASTER);    // Set vector offset for master PIC
    outportb(PIC_DATA_MASTER, ICW_3_MASTER);    // Tell master PIC there is a slave
    outportb(PIC_DATA_MASTER, ICW_4);    // Enable master PIC

    // Disable the slave PIC
    outportb(PIC_COMMAND_SLAVE, ICW_1);   // Initialize command
    outportb(PIC_DATA_SLAVE, ICW_2_SLAVE);      // Set vector offset for slave PIC
    outportb(PIC_DATA_SLAVE, ICW_3_SLAVE);      // Tell slave PIC its cascade identity
    outportb(PIC_DATA_SLAVE, ICW_4);      // Enable slave PIC

    // Now both PICs are disabled, so we can safely ignore interrupts
    outportb(PIC_COMMAND_MASTER, 0xFF); // Mask all interrupts on
    outportb(PIC_COMMAND_SLAVE, 0xFF);  // Mask all interrupts on slave
    // This effectively disables the PICs, preventing any further interrupts
    // from being processed until they are re-enabled.
}