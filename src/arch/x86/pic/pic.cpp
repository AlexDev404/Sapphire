#include <arch/x86/pic/pic.hpp>

void pic_remap(uint8_t offset1, uint8_t offset2)
{
	// Save the current masks (read from DATA ports)
	uint8_t mask1 = inb(PIC1_DATA);
	uint8_t mask2 = inb(PIC2_DATA);

	// ICW1 — start init sequence on both PICs
	// Send (ICW1_INIT | ICW1_ICW4) to
	// PIC1_COMMAND and PIC2_COMMAND
	// Call io_wait() after each outb for safety
	io_wait(outb, PIC2_COMMAND, ICW1_INIT | ICW1_ICW4);
	io_wait(outb, PIC1_COMMAND, ICW1_INIT | ICW1_ICW4);

	// ICW2 — set vector offsets
	// Send offset1 to PIC1_DATA
	// (master starts at interrupt 'offset1')
	// Send offset2 to PIC2_DATA
	// (slave starts at interrupt 'offset2')
	io_wait(outb, PIC1_DATA, offset1);
	io_wait(outb, PIC2_DATA, offset2);

	// ICW3 — tell PICs about each other
	// Master: send 0x04 to PIC1_DATA
	// (slave is on IRQ line 2, which is bit 2 = 0x04)
	// Slave:  send 0x02 to PIC2_DATA
	// (its cascade identity = 2)
	io_wait(outb, PIC1_DATA, 0x04);
	io_wait(outb, PIC2_DATA, 0x02);

	// ICW4 — set 8086 mode
	// Send ICW4_8086 to both DATA ports
	io_wait(outb, PIC1_DATA, ICW4_8086);
	io_wait(outb, PIC2_DATA, ICW4_8086);

	// Restore the saved masks
	// (write them back to DATA ports)
	io_wait(outb, PIC1_DATA, mask1);
	io_wait(outb, PIC2_DATA, mask2);
}

void pic_send_eoi(uint8_t irq)
{
	// If irq >= 8, it came from the slave PIC
	// Send EOI to slave PIC first
	if (irq >= 8)
	{
		outb(PIC2_COMMAND, PIC_EOI);
	}

	// Always send EOI to master PIC
	outb(PIC1_COMMAND, PIC_EOI);
}
