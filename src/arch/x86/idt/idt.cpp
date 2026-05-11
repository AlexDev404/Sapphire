#include <arch/x86/idt/idt.hpp>
#include <arch/x86/pic/pic.hpp>
#include <arch/x86/gdt/gdt.hpp>
#include <driver/fb/kprint.hpp>

// The IDT — 256 entries
static IDTEntry g_IDT[IDT_MAX_ENTRIES];

// The IDT descriptor (loaded by lidt)
static IDTDescriptor g_IDTDescriptor;

// Human-readable exception names for debugging
static const char *exception_names[] = {
	"Division by Zero",        // 0
	"Debug",                   // 1
	"Non-Maskable Interrupt",  // 2
	"Breakpoint",              // 3
	"Overflow",                // 4
	"Bound Range Exceeded",    // 5
	"Invalid Opcode",          // 6
	"Device Not Available",    // 7
	"Double Fault",            // 8
	"Coprocessor Segment",     // 9
	"Invalid TSS",             // 10
	"Segment Not Present",     // 11
	"Stack-Segment Fault",     // 12
	"General Protection Fault",// 13
	"Page Fault",              // 14
	"Reserved",                // 15
	"x87 FP Exception",       // 16
	"Alignment Check",         // 17
	"Machine Check",           // 18
	"SIMD FP Exception",      // 19
	"Virtualization Exception",// 20
	"Control Protection",      // 21
	"Reserved", "Reserved", "Reserved", "Reserved", "Reserved", "Reserved",
	"Hypervisor Injection",    // 28
	"VMM Communication",       // 29
	"Security Exception",      // 30
	"Reserved"                 // 31
};

// Set a single IDT entry.
//
// TODO: Fill in the IDTEntry fields:
//   - offset_low:  low 16 bits of handler address
//   - selector:    code segment selector (GDT_CODE_SEGMENT)
//   - reserved:    always 0
//   - flags:       the flags parameter (e.g. IDT_FLAG_GATE_INTERRUPT)
//   - offset_high: high 16 bits of handler address
//
// Hint: look at how GDT_ENTRY macro splits base into low/middle/high.
//       Same idea here but simpler — only low and high.
static void idt_set_entry(uint8_t index, uint32_t handler, uint8_t flags)
{
	// TODO: Fill in g_IDT[index]
	// g_IDT[index].offset_low  = ???
	// g_IDT[index].selector    = ???
	// g_IDT[index].reserved    = ???
	// g_IDT[index].flags       = ???
	// g_IDT[index].offset_high = ???
}

void init_idt()
{
	// TODO Step 1: Set up the IDT descriptor
	// g_IDTDescriptor.limit = ???  (hint: sizeof(g_IDT) - 1, same pattern as GDT)
	// g_IDTDescriptor.base  = ???  (hint: address of g_IDT)

	// TODO Step 2: Register all CPU exception handlers (ISR 0-31)
	// Call idt_set_entry for each one with IDT_FLAG_GATE_INTERRUPT
	// Example: idt_set_entry(0, (uint32_t)isr0, IDT_FLAG_GATE_INTERRUPT);
	//          idt_set_entry(1, (uint32_t)isr1, IDT_FLAG_GATE_INTERRUPT);
	//          ... up to isr31
	// ... your code here ...

	// TODO Step 3: Remap the PIC
	// Call pic_remap with offsets 0x20 (32) and 0x28 (40)
	// ... your code here ...

	// TODO Step 4: Register all IRQ handlers (IRQ 0-15 → interrupts 32-47)
	// Example: idt_set_entry(32, (uint32_t)irq0, IDT_FLAG_GATE_INTERRUPT);
	//          idt_set_entry(33, (uint32_t)irq1, IDT_FLAG_GATE_INTERRUPT);
	//          ... up to irq15 at entry 47
	// ... your code here ...

	// TODO Step 5: Load the IDT
	// Call _load_idt(&g_IDTDescriptor);
	// ... your code here ...
}

// This is the C++ handler that ALL interrupt stubs call.
// The asm stub pushes an InterruptFrame onto the stack and passes a pointer to it.
extern "C" void isr_handler(InterruptFrame *frame)
{
	uint32_t intno = frame->interrupt_number;

	if (intno < 32)
	{
		// TODO: CPU exception
		// Print the exception name and number, then halt
		// Hint: use exception_names[intno] for the name
		// kprintf("Exception %d: %s\n", intno, exception_names[intno]);
		// kprintf("Error code: 0x%x\n", frame->error_code);
		// kprintf("EIP: 0x%x\n", frame->eip);
		// Then halt: for(;;) asm volatile("cli; hlt");
		// ... your code here ...
	}
	else if (intno >= 32 && intno < 48)
	{
		// TODO: Hardware IRQ
		// The IRQ number is intno - 32
		// For now, just silently acknowledge it
		// Later you can add specific handlers (timer, keyboard, etc.)
		//
		// IMPORTANT: You MUST send EOI to the PIC or no more IRQs will fire!
		// Call pic_send_eoi(intno - 32);
		// ... your code here ...
	}
}
