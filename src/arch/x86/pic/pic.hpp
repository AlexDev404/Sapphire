#pragma once
#include <stdint.h>

// PIC I/O ports
#define PIC1_COMMAND 0x20 // Master
#define PIC1_DATA 0x21
#define PIC2_COMMAND 0xA0 // Slave
#define PIC2_DATA 0xA1

// ICW = Initialization Command Word (the steps to configure the PIC)
#define ICW1_INIT 0x10 // bit 4: "I want to initialize"
#define ICW1_ICW4 0x01 // bit 0: "ICW4 will follow"
#define ICW4_8086 0x01 // 8086 mode (not 8080)

// End of Interrupt command
#define PIC_EOI 0x20

// Remap the PIC so IRQs don't overlap CPU exceptions.
//
// By default:
//   Master PIC (IRQ 0-7)  → interrupts 0x08–0x0F  (overlaps exceptions!)
//   Slave PIC  (IRQ 8-15) → interrupts 0x70–0x77
//
// After remapping with offset1=0x20, offset2=0x28:
//   Master PIC (IRQ 0-7)  → interrupts 0x20–0x27 (32–39)
//   Slave PIC  (IRQ 8-15) → interrupts 0x28–0x2F (40–47)
//
// The PIC initialization sequence is 4 ICWs (Initialization Command Words):
//   ICW1: Tell PIC to start init sequence (sent to COMMAND port)
//   ICW2: Set the interrupt vector offset (sent to DATA port)
//   ICW3: Tell PICs about each other (master/slave wiring)
//   ICW4: Set mode (8086 mode)
//
// Steps:
//   1. Send ICW1 (ICW1_INIT | ICW1_ICW4) to both COMMAND ports
//   2. Send ICW2 (offset) to both DATA ports
//   3. Send ICW3: tell master that slave is on IRQ2 (0x04), tell slave its cascade identity (0x02)
//   4. Send ICW4 (ICW4_8086) to both DATA ports
//   5. Set interrupt masks on both DATA ports (0x0 = all enabled)
//
// Hint: use outb(port, value) to write to a port
void pic_remap(uint8_t offset1, uint8_t offset2);

// Send End of Interrupt to the PIC.
// If the IRQ came from the slave PIC (irq >= 8), send EOI to both slave AND master.
// Otherwise just send to master.
//
// Steps:
//   1. If irq >= 8, send PIC_EOI to PIC2_COMMAND
//   2. Always send PIC_EOI to PIC1_COMMAND
void pic_send_eoi(uint8_t irq);

// Port I/O helpers — you'll need these
extern "C" void outb(uint16_t port, uint8_t value);
extern "C" uint8_t inb(uint16_t port);

// Small delay for PIC (some hardware needs a moment between commands)
static inline void io_wait()
{
	outb(0x80, 0); // port 0x80 is used for POST codes — writing to it is a safe no-op delay
}

static inline void io_wait(void (*fn)(uint16_t, uint8_t), uint16_t port, uint8_t value)
{
	fn(port, value);
	io_wait();
}