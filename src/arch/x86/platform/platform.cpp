#include <arch/noarch/platform/platform.hpp>
#include <arch/x86/gdt/gdt.hpp>

void kmain(); // Messy, but we'll see it once we link.

extern "C" void platform_init(){
    init_gdt();
    kmain();
}

void platform_interrupts_enable()  { asm volatile("sti"); }
void platform_interrupts_disable() { asm volatile("cli"); }
