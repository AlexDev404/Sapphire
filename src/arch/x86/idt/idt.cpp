#include <arch/x86/idt/idt.hpp>
#include <arch/x86/pic/pic.hpp>
#include <arch/x86/gdt/gdt.hpp>
#include <driver/fb/kprint.hpp>

// The IDT — 256 entries (hardware table, loaded by lidt)
static IDTEntry g_IDT[IDT_MAX_ENTRIES];

// The IDT descriptor (loaded by lidt)
static IDTDescriptor g_IDTDescriptor;

// Software dispatch table — function pointers, one per interrupt.
// isr_handler looks up handlers[intno] and calls it.
// Default handler for unregistered interrupts.
static void default_handler(InterruptFrame *frame);
static void default_exception_handler(InterruptFrame *frame);
static void default_irq_handler(InterruptFrame *frame);

// Declare the handler table — 256 function pointers, all initialized to default_handler
// Hint: static isr_t handlers[IDT_MAX_ENTRIES] = { [0 ... 255] = default_handler };
// (This is the GNU range initializer you wanted to use!)
static isr_t handlers[IDT_MAX_ENTRIES];

// Human-readable exception names for debugging
static const char *exception_names[] = {
	"Division by Zero",			// 0
	"Debug",					// 1
	"Non-Maskable Interrupt",	// 2
	"Breakpoint",				// 3
	"Overflow",					// 4
	"Bound Range Exceeded",		// 5
	"Invalid Opcode",			// 6
	"Device Not Available",		// 7
	"Double Fault",				// 8
	"Coprocessor Segment",		// 9
	"Invalid TSS",				// 10
	"Segment Not Present",		// 11
	"Stack-Segment Fault",		// 12
	"General Protection Fault", // 13
	"Page Fault",				// 14
	"Reserved",					// 15
	"x87 FP Exception",			// 16
	"Alignment Check",			// 17
	"Machine Check",			// 18
	"SIMD FP Exception",		// 19
	"Virtualization Exception", // 20
	"Control Protection",		// 21
	"Reserved", "Reserved", "Reserved", "Reserved", "Reserved", "Reserved",
	"Hypervisor Injection", // 28
	"VMM Communication",	// 29
	"Security Exception",	// 30
	"Reserved"				// 31
};

// Set a single IDT entry.
//
// The IDTEntry fields:
//   - offset_low:  low 16 bits of handler address  → handler & 0xFFFF
//   - selector:    code segment selector            → GDT_CODE_SEGMENT
//   - reserved:    always 0
//   - flags:       the flags parameter
//   - offset_high: high 16 bits of handler address  → (handler >> 16) & 0xFFFF
void idt::set_entry(uint8_t index, uint32_t handler, uint8_t flags)
{
	// Fill in g_IDT[index]
	g_IDT[index].offset_low = handler & 0xFFFF;
	g_IDT[index].selector = GDT_CODE_SEGMENT;
	g_IDT[index].reserved = false;
	g_IDT[index].flags = flags;
	g_IDT[index].offset_high = (handler >> 16) & 0xFFFF;
}

void idt::init()
{
	// Step 1: Set up the IDT descriptor
	g_IDTDescriptor.limit = sizeof(g_IDT) - 1;
	g_IDTDescriptor.base = (uint32_t)&g_IDT;

	// Fill the ISR handlers
	fill(handlers, default_handler, 0, 256);
	fill(handlers, default_exception_handler, 0, 32);
	fill(handlers, default_irq_handler, 32, 48);
	// Step 2: Register all CPU exception ISR stubs (0-31) in the IDT
	// These are the ASM stubs, NOT the C++ handlers.
	// Example: idt::set_entry(0, (uint32_t)isr0, IDT_FLAG_GATE_INTERRUPT);
	//          idt::set_entry(1, (uint32_t)isr1, IDT_FLAG_GATE_INTERRUPT);
	//          ... up to isr31
	for (int i = 0; i < 32; i++)
	{
		idt::set_entry(i, (uint32_t)(isr[i]), IDT_FLAG_GATE_INTERRUPT);
	}

	for (int i = 32; i < 48; i++)
	{
		idt::set_entry(i, (uint32_t)(irqp[i - 32]), IDT_FLAG_GATE_INTERRUPT);
	}

	// Special case: Syscall handler
	idt::set_entry(128, (uint32_t)(isr128), IDT_FLAG_GATE_INTERRUPT);

	// Step 3: Remap the PIC
	// Call pic_remap with offsets 0x20 (32) and 0x28 (40)
	pic_remap(32, 40);

	//  Step 4: Load the IDT
	// Call _load_idt(&g_IDTDescriptor);
	_load_idt(&g_IDTDescriptor);
}

// Register a custom handler for a specific interrupt.
// Example: register_interrupt_handler(14, my_page_fault_handler);
void interrupts::register_handler(uint8_t intno, isr_t handler)
{
	handlers[intno] = handler;
}

void interrupts::enable() { asm volatile("sti"); }
void interrupts::disable() { asm volatile("cli"); }

// This is the C++ handler that ALL asm stubs call.
// It dispatches to the appropriate function pointer in handlers[].
extern "C" void isr_handler(InterruptFrame *frame)
{

	// Look up the handler in the table and call it
	uint32_t intno = frame->interrupt_number;
	if (handlers[intno])
		handlers[intno](frame);

	// For IRQs (intno >= 32 && intno < 48)
	// you MUST send EOI after the handler runs.
	// Call pic_send_eoi(intno - 32);
	if (intno >= 32 && intno < 48)
		pic_send_eoi(intno - 32);
}

// Default exception handler — prints info and halts
static void default_exception_handler(InterruptFrame *frame)
{
	kprintf("Exception %d: %s\n", frame->interrupt_number, exception_names[frame->interrupt_number]);
	kprintf("Error code: 0x%x\n", frame->error_code);
	kprintf("EIP: 0x%x\n", frame->eip);
	while (true)
		asm volatile("cli; hlt");
}

// Default IRQ handler — silently acknowledges the interrupt
// (EOI is sent by isr_handler after this returns)
static void default_irq_handler(InterruptFrame *frame)
{
	(void)frame; // unused — just let isr_handler send EOI
}

// Default handler — does nothing (for interrupts 48-255 that nobody cares about)
static void default_handler(InterruptFrame *frame)
{
	(void)frame; // unused
}
