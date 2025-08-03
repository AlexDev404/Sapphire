#include "init.hpp"
#include <platform/idt.hpp>
#include <devices/debug/debugcon.h>
#include <graphics/print_text.hpp>
#include <devices/apic/apic.hpp>

// Forward declaration for the interrupt handler
extern "C" void isr0_handler(void);
extern "C" void spurious_interrupt_handler(void);
extern "C" void timer_interrupt_handler(void);
extern "C" void isr33_handler(void);

// Timer tick counter
static volatile uint32_t timer_ticks = 0;

// Your C handler for interrupt 0 (divide by zero)
extern "C" void isr0_c(void)
{
    // Send the debug output
    send_debug("EXCEPTION: Divide by zero detected!\n");
    print_text("EXCEPTION: Divide by zero detected!", COLOR_RED, COLOR_BLACK, 30, 220);

    // In a real OS, you would:
    // 1. Check if this happened in user space or kernel space
    // 2. If user space: terminate the process
    // 3. If kernel space: kernel panic

    // We'll halt here once we catch the interrupt
    while (1) {
        // Do nothing
    }
    return;
}

extern "C" void isr13_c(char error_code, unsigned short vector) {
    // Send the debug output for the general protection fault
    send_debug("EXCEPTION: General Protection Fault (vector: 0x");
    
    // Convert vector to hex string
    send_debug(vector < 10 ? "0" : "");
    
    send_debug(") with error code: ");
    
    // Convert error code to hex string
    send_debug(error_code < 10 ? "0" : "");
    
    send_debug("\n");
    
    print_text("EXCEPTION: General Protection Fault occurred!", COLOR_RED, COLOR_BLACK, 30, 220);

    // Halt the system
    while (1) {
        // Do nothing
    }
}

// C handler for spurious interrupts (vector 0xFF)
extern "C" void spurious_interrupt_c(void)
{
    // Send debug output (spurious interrupts are usually harmless)
    send_debug("SPURIOUS INTERRUPT: Received spurious interrupt (0xFF)\n");
    
    // NOTE: For spurious interrupts, we do NOT send an EOI to the LAPIC
    // The hardware handles this automatically for spurious interrupts
    
    // Simply return - spurious interrupts are typically safe to ignore
    return;
}

// Timer interrupt handler (vector 0x20)
extern "C" void timer_interrupt_c(void)
{
    timer_ticks++;
    const int column = timer_ticks % 89; // Assuming 89 columns for the console
    const int row = timer_ticks / 89; // Calculate row based on ticks

    // Send immediate debug output for every interrupt
    print_at(".", COLOR_YELLOW, COLOR_BLACK, column, row);

    // Send EOI to acknowledge the interrupt
    lapic_send_eoi();
}


extern "C" void isr33_c(void)
{
    // Keyboard interrupt handler (vector 0x21)
    print_text("Keyboard interrupt received (vector 0x21)\n", COLOR_YELLOW, COLOR_BLACK, 30, 240);
    lapic_send_eoi(); // Acknowledge the interrupt
    return;
}

/*
 * Init function
 * The purpose of this function is to initialize all devices and the environment itself
 */
void init(void)
{
    // Initialize IDT with basic exception handlers
    idt_set_gate(0, (unsigned long *)isr0_handler, 0); // Divide by zero
    idt_set_gate(0xFF, (unsigned long *)spurious_interrupt_handler, 0); // Spurious interrupt
    idt_set_gate(0x20, (unsigned long *)timer_interrupt_handler, 0); // Timer interrupt
    idt_set_gate(0x21, (unsigned long *)isr33_handler, 0); // Keyboard interrupt
    idt_init();

    // IDT is now active and ready to handle exceptions
    print_text("IDT Initialized - Exception handling active", COLOR_GREEN, COLOR_BLACK, 30, 200);

    // Try inline assembly to force the division
// #ifndef _MSC_VER
//     asm volatile(
//         "movl $100, %%eax;" // Load 100 into EAX
//         "movl $0, %%ebx;"   // Load 0 into EBX
//         "divl %%ebx;"       // Divide EAX by EBX (100/0) - should trigger interrupt 0
//         :
//         :
//         : "eax", "ebx", "edx" // Clobbered registers
//     );
// #endif


     // Test with software interrupt (no instruction skipping needed)
    // #ifndef _MSC_VER
    // asm volatile ("int $0" ::: "memory");  // Call interrupt 0 directly
    // #endif

    disable_pic8259(); // Disable the legacy PICs
    print_text("Legacy PICs disabled", COLOR_YELLOW, COLOR_BLACK, 30, 240);
    
    enable_lapic();    // Enable the Local APIC
    print_text("LAPIC enabled and configured", COLOR_GREEN, COLOR_BLACK, 30, 260);
    
    // Get and display LAPIC ID
    uint32_t lapic_id = lapic_get_id();
    send_debug("LAPIC ID: ");
    // Simple hex conversion for debugging
    char hex_digits[] = "0123456789ABCDEF";
    char id_str[3];
    id_str[0] = hex_digits[(lapic_id >> 4) & 0xF];
    id_str[1] = hex_digits[lapic_id & 0xF];
    id_str[2] = '\0';
    send_debug(id_str);
    send_debug("\n");
    
    // Set up LAPIC timer for periodic interrupts
    // Vector 0x20, initial count 100000 (smaller for more frequent interrupts), divide by 16, periodic mode
    send_debug("Setting up LAPIC timer...\n");
    lapic_timer_setup(0x20, 100000, LAPIC_TIMER_DIVIDE_BY_16, LAPIC_TIMER_PERIODIC);
    print_text("LAPIC timer configured (periodic mode)", COLOR_CYAN, COLOR_BLACK, 30, 280);
    send_debug("LAPIC timer started - periodic interrupts enabled\n");
    
    // Check if timer is actually counting down
    uint32_t current_count = lapic_get_timer_current_count();
    send_debug("Timer current count: ");
    char count_str[16];
    int i = 0;
    if (current_count == 0) {
        count_str[i++] = '0';
    } else {
        char temp_buf[16];
        int j = 0;
        uint32_t temp = current_count;
        while (temp > 0) {
            temp_buf[j++] = '0' + (temp % 10);
            temp /= 10;
        }
        for (int k = j - 1; k >= 0; k--) {
            count_str[i++] = temp_buf[k];
        }
    }
    count_str[i] = '\0';
    send_debug(count_str);
    send_debug("\n");
    
    // Enable interrupts to allow timer interrupts to fire
    send_debug("Interrupts enabled - timer should start firing\n");
    #ifndef _MSC_VER
    asm volatile("sti");
    #endif
    
    // Test if interrupt handling is working by triggering a software interrupt
    // send_debug("Testing software interrupt...\n");
    // #ifndef _MSC_VER
    // asm volatile("int $0x20"); // Trigger timer interrupt manually
    // #endif
    // send_debug("Software interrupt test complete\n");
}
