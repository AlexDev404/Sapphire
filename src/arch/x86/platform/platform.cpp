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
		/**
		 * Current output
		 * Memory map found!
		 * Memory map entry count: 6
		 * Entry 1: Base=0x0, Length=0x9FC00, Type=1
		 * Entry 2: Base=0x9FC00, Length=0x400, Type=2
		 * Entry 3: Base=0xF0000, Length=0x10000, Type=2
		 * Entry 4: Base=0x100000, Length=0x7EE0000, Type=1  <----------- Usable block (since 32-bit, max length is 4 GB - 1 MB)
		 * Entry 5: Base=0x7FE0000, Length=0x20000, Type=2
		 * Entry 6: Base=0xFFF00000, Length=0x40000, Type=2
		 * 
		 * ┌─────┬────────────┬─────────┬──────────┬─────────────────────────────────────┐                                  
		 * │  #  │    Base    │ Length  │   Type   │               Meaning               │
		 * ├─────┼────────────┼─────────┼──────────┼─────────────────────────────────────┤                                  
		 * │ 1   │ 0x0        │ 640 KB  │ Usable   │ Conventional memory                 │
		 * ├─────┼────────────┼─────────┼──────────┼─────────────────────────────────────┤
		 * │ 2   │ 0x9FC00    │ 1 KB    │ Reserved │ Extended BIOS Data Area (EBDA)      │
		 * ├─────┼────────────┼─────────┼──────────┼─────────────────────────────────────┤
		 * │ 3   │ 0xF0000    │ 64 KB   │ Reserved │ BIOS ROM shadow                     │
		 * ├─────┼────────────┼─────────┼──────────┼─────────────────────────────────────┤
		 * │ 4   │ 0x100000   │ ~127 MB │ Usable   │ Main RAM (above 1 MB)               │
		 * ├─────┼────────────┼─────────┼──────────┼─────────────────────────────────────┤
		 * │ 5   │ 0x7FE0000  │ 128 KB  │ Reserved │ BIOS reserved (top of RAM)          │
		 * ├─────┼────────────┼─────────┼──────────┼─────────────────────────────────────┤
		 * │ 6   │ 0xFFF00000 │ 256 KB  │ Reserved │ High BIOS area (4 GB - 1 MB region) │
		 * └─────┴────────────┴─────────┴──────────┴─────────────────────────────────────┘
		 * 
		 */
		kprintf("Entry %d: Base=0x%x, Length=0x%x, Type=%d\n", i + 1, 
			(uint32_t)(entry->base), (uint32_t)(entry->length), (uint32_t)(entry->type));
	} while (++i < boot_info->e820_entry_count);
	init_gdt();
}

void platform_interrupts_enable() { asm volatile("sti"); }
void platform_interrupts_disable() { asm volatile("cli"); }
