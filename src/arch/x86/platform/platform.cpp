#include <arch/noarch/platform/platform.hpp>
#include <arch/x86/gdt/gdt.hpp>
#include <arch/x86/platform/boot_info.hpp>
#include <driver/fb/kprint.hpp>
#include <stdint.h>

extern "C" void platform_init(uint32_t boot_info_ptr)
{
    BootInfo *boot_info = reinterpret_cast<BootInfo *>(boot_info_ptr);
    kprint(boot_info->e820_entry_count > 0 ? "Memory map found!\n" : "No memory map found!\n");
    kprintf("Memory map entry count: %d\n", boot_info->e820_entry_count);

    uint32_t i = 0;
    do
    {
        E820Entry *entry = &boot_info->e820_map[i];
        kprintf("Entry %d: Base=0x%x, Length=0x%x, Type=%d\n", i, ((uint32_t)(entry->base) + 1), (uint32_t)(entry->length), (uint32_t)(entry->type));
    } while (++i < boot_info->e820_entry_count);
    init_gdt();
}

void platform_interrupts_enable() { asm volatile("sti"); }
void platform_interrupts_disable() { asm volatile("cli"); }
