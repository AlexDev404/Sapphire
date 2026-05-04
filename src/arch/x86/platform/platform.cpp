#include <arch/noarch/platform/platform.hpp>
#include <arch/x86/gdt/gdt.hpp>


extern "C" void platform_init(){
    init_gdt();
}

void platform_interrupts_enable()  { asm volatile("sti"); }
void platform_interrupts_disable() { asm volatile("cli"); }
