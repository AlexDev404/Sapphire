#include "init.hpp"
#include "../../devices/idt/idt.hpp"
#include "../../devices/debug/debugcon.h"
#include "../../graphics/print_text.h"

// Forward declaration for the interrupt handler
extern "C" void isr0_handler(void);
extern "C" void isr0_c(void);

// Your C handler for interrupt 0 (divide by zero)
extern "C" void isr0_c(void)
{
    // Use interrupt-safe debug output
    interrupt_debug_char('E');
    interrupt_debug_char('R');
    interrupt_debug_char('R');
    interrupt_debug_char(':');
    interrupt_debug_char(' ');
    interrupt_debug_char('D');
    interrupt_debug_char('b');
    interrupt_debug_char('0');
    interrupt_debug_char('\n');


    // The regular send_debug might not work in interrupt context
    // send_debug("EXCEPTION: Divide by zero detected!\n");

    // In a real OS, you would:
    // 1. Check if this happened in user space or kernel space
    // 2. If user space: terminate the process
    // 3. If kernel space: kernel panic

    // We'll halt here once we catch the interrupt
    while (1)
    {
    }
    return;
}

// Interrupt Service Routine for interrupt 0 (divide by zero)
asm(
    ".global isr0_handler    \n"
    "isr0_handler:           \n"
    "    cli                 \n" // Disable interrupts
    "    pusha               \n" // Save all registers
    "    call isr0_c         \n" // Call the C handler
    "    popa                \n" // Restore all registers
    "    sti                 \n" // Re-enable interrupts
    "    iret                \n" // Return from interrupt
);

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

// Also test a simple function that takes a different parameter type
void test_with_int(int value)
{
    send_debug("INT TEST: Function with int parameter\n");
}

// Test function with no parameters to see if the issue is parameter-related
void init_no_params(void)
{
    send_debug("NO PARAMS TEST: Function entered\n");
}
