#include <arch/noarch/platform/platform.hpp>
#include <arch/x86/gdt/gdt.hpp>
#include <arch/x86/platform/boot_info.hpp>
#include <driver/fb/kprint.hpp>

extern "C" void platform_init(uint32_t boot_info_ptr) {
    BootInfo* boot_info = reinterpret_cast<BootInfo*>(boot_info_ptr);
    kprint(boot_info->e820_entry_count > 0 ? "Memory map found!\n" : "No memory map found!\n");    
    init_gdt();
}

void platform_interrupts_enable()  { asm volatile("sti"); }
void platform_interrupts_disable() { asm volatile("cli"); }
