#include "apic.hpp"

// Global variable to store LAPIC base address
static uint32_t* g_lapic_base = nullptr;

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
void enable_lapic() {
    // Read the APIC base MSR (IA32_APIC_BASE = 0x1B)
    uint32_t eax, edx;
    #ifdef __GNUC__
    asm volatile("rdmsr" : "=a"(eax), "=d"(edx) : "c"(IA32_APIC_BASE_MSR));
    #endif 
    
    // Combine the results (eax = lower 32 bits, edx = upper 32 bits)
    uint64_t apic_base = ((uint64_t)edx << 32) | eax;
    
    // Set the APIC global enable bit (bit 11)
    apic_base |= IA32_APIC_BASE_ENABLE;

    #ifdef __GNUC__
    asm volatile("wrmsr" : : "a"((uint32_t)apic_base), "d"((uint32_t)(apic_base >> 32)), "c"(IA32_APIC_BASE_MSR));
    #endif

    // Get the LAPIC base address from bits 12-31 (mask with 0xFFFFF000)
    g_lapic_base = (uint32_t*)(apic_base & 0xFFFFF000);
    
    // Set up the spurious vector register
    // Bits 0-7: Spurious vector (0xFF for compatibility with old CPUs)
    // Bit 8: Software enable bit (1 to enable)
    // Bit 9: Focus processor checking (0 to disable)
    uint32_t spurious_vector = 0xFF | (1 << 8);
    
    // Write to the spurious vector register (offset 0xF0)
    g_lapic_base[IA32_LAPIC_SPURIOUS_OFFSET / 4] = spurious_vector;

    // Clear any pending errors
    g_lapic_base[IA32_LAPIC_ESR_OFFSET / 4] = 0;
    g_lapic_base[IA32_LAPIC_ESR_OFFSET / 4] = 0;

    // Disable all LVT entries initially (mask them)
    g_lapic_base[IA32_LAPIC_TIMER_LVT_OFFSET / 4] = LAPIC_LVT_MASKED;
    g_lapic_base[IA32_LAPIC_THERMAL_LVT_OFFSET / 4] = LAPIC_LVT_MASKED;
    g_lapic_base[IA32_LAPIC_PERF_LVT_OFFSET / 4] = LAPIC_LVT_MASKED;
    g_lapic_base[IA32_LAPIC_LINT0_LVT_OFFSET / 4] = LAPIC_LVT_MASKED;
    g_lapic_base[IA32_LAPIC_LINT1_LVT_OFFSET / 4] = LAPIC_LVT_MASKED;
    g_lapic_base[IA32_LAPIC_ERROR_LVT_OFFSET / 4] = LAPIC_LVT_MASKED;

    // Set the task priority register to 0 (accept all interrupts)
    g_lapic_base[IA32_LAPIC_TPR_OFFSET / 4] = 0;

    // Now the LAPIC is enabled and ready to handle interrupts
}

uint32_t* get_lapic_base() {
    if (g_lapic_base == nullptr) {
        // Read the APIC base MSR to get the current base address
        uint32_t eax, edx;
        #ifdef __GNUC__
        asm volatile("rdmsr" : "=a"(eax), "=d"(edx) : "c"(IA32_APIC_BASE_MSR));
        #endif 
        
        uint64_t apic_base = ((uint64_t)edx << 32) | eax;
        g_lapic_base = (uint32_t*)(apic_base & 0xFFFFF000);
    }
    return g_lapic_base;
}

void lapic_send_eoi() {
    // Send End of Interrupt (EOI) signal to the LAPIC
    // This tells the LAPIC that the current interrupt has been handled
    uint32_t* lapic_base = get_lapic_base();
    lapic_base[IA32_LAPIC_EOI_OFFSET / 4] = 0; // Write any value to EOI register
}

uint32_t lapic_get_id() {
    // Get the LAPIC ID from the LAPIC ID register
    uint32_t* lapic_base = get_lapic_base();
    return (lapic_base[IA32_LAPIC_ID_OFFSET / 4] >> 24) & 0xFF;
}

void lapic_timer_setup(uint32_t vector, uint32_t initial_count, uint32_t divide_config, uint32_t mode) {
    uint32_t* lapic_base = get_lapic_base();
    
    // First, mask the timer to prevent spurious interrupts during setup
    lapic_base[IA32_LAPIC_TIMER_LVT_OFFSET / 4] = LAPIC_LVT_MASKED;
    
    // Set the divide configuration for the timer
    lapic_base[IA32_LAPIC_TIMER_DIVIDE_CONFIG_OFFSET / 4] = divide_config;
    
    // Set up the LVT timer entry
    // Bits 0-7: Vector number
    // Bits 17-18: Timer mode (00 = one-shot, 01 = periodic, 10 = TSC-deadline)
    // Bit 16: Mask bit (0 = not masked, 1 = masked)
    uint32_t lvt_timer = vector | (mode << 17);
    lapic_base[IA32_LAPIC_TIMER_LVT_OFFSET / 4] = lvt_timer;
    
    // Set the initial count (this starts the timer)
    lapic_base[IA32_LAPIC_TIMER_INITIAL_COUNT_OFFSET / 4] = initial_count;
}

void lapic_set_timer_initial_count(uint32_t count) {
    uint32_t* lapic_base = get_lapic_base();
    lapic_base[IA32_LAPIC_TIMER_INITIAL_COUNT_OFFSET / 4] = count;
}

uint32_t lapic_get_timer_current_count() {
    uint32_t* lapic_base = get_lapic_base();
    return lapic_base[IA32_LAPIC_TIMER_CURRENT_COUNT_OFFSET / 4];
}

void lapic_disable_timer() {
    uint32_t* lapic_base = get_lapic_base();
    // Mask the timer LVT entry to disable it
    lapic_base[IA32_LAPIC_TIMER_LVT_OFFSET / 4] = LAPIC_LVT_MASKED;
    // Stop the timer by setting initial count to 0
    lapic_base[IA32_LAPIC_TIMER_INITIAL_COUNT_OFFSET / 4] = 0;
}