#include "init.hpp"
#include <platform/idt.hpp>
#include <devices/debug/debugcon.h>
#include <graphics/print_text.h>

// Forward declaration for the interrupt handler
extern "C" void isr0_handler(void);

// Your C handler for interrupt 0 (divide by zero)
extern "C" void isr0_c(void)
{
    // Send the debug output
    send_debug("EXCEPTION: Divide by zero detected!\n");

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

/*
 * Init function
 * The purpose of this function is to initialize all devices and the environment itself
 */
void init(const VbeModeInfo *vbe_mode_info)
{
    // Initialize IDT with basic exception handlers
    idt_set_gate(0, (unsigned long *)isr0_handler, 0); // Divide by zero
    idt_init();

    // IDT is now active and ready to handle exceptions
    const unsigned char text[] = "IDT Initialized - Exception handling active";
    print_text(text, COLOR_GREEN, COLOR_BLACK, 30, 200, vbe_mode_info);

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
}
