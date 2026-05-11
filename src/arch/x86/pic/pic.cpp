#include <arch/x86/pic/pic.hpp>

void pic_remap(uint8_t offset1, uint8_t offset2)
{
	// TODO: Save the current masks (read from DATA ports)
	// uint8_t mask1 = inb(PIC1_DATA);
	// uint8_t mask2 = inb(PIC2_DATA);

	// TODO: ICW1 — start init sequence on both PICs
	// Send (ICW1_INIT | ICW1_ICW4) to PIC1_COMMAND and PIC2_COMMAND
	// Call io_wait() after each outb for safety
	// ... your code here ...

	// TODO: ICW2 — set vector offsets
	// Send offset1 to PIC1_DATA (master starts at interrupt 'offset1')
	// Send offset2 to PIC2_DATA (slave starts at interrupt 'offset2')
	// ... your code here ...

	// TODO: ICW3 — tell PICs about each other
	// Master: send 0x04 to PIC1_DATA (slave is on IRQ line 2, which is bit 2 = 0x04)
	// Slave:  send 0x02 to PIC2_DATA (its cascade identity = 2)
	// ... your code here ...

	// TODO: ICW4 — set 8086 mode
	// Send ICW4_8086 to both DATA ports
	// ... your code here ...

	// TODO: Restore the saved masks (write them back to DATA ports)
	// ... your code here ...
}

void pic_send_eoi(uint8_t irq)
{
	// If irq >= 8, it came from the slave PIC — send EOI to slave first
	// ... your code here ...

	// TODO: Always send EOI to the master PIC
	// ... your code here ...
}
