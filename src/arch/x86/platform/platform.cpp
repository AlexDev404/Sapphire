#include <arch/noarch/platform/platform.hpp>
#include <arch/x86/gdt/gdt.hpp>
#include <arch/x86/platform/boot_info.hpp>
#include <arch/x86/fb/fb.h>
#include <arch/x86/paging/paging.hpp>
#include <arch/x86/memory/memory_map.hpp>
#include <driver/fb/kprint.hpp>
#include <stdint.h>

extern "C" void platform_init(uint32_t boot_info_ptr)
{
	BootInfo *boot_info = reinterpret_cast<BootInfo *>(boot_info_ptr);
	kprint(boot_info->e820_entry_count > 0 ? "Memory map found!\n" : "No memory map found!\n");

	// Set up paging
	PageDirectory pd;
	pd.init(boot_info->page_directory);

	// Identity-map critical regions
	pd.identity_map_range(0x00000000, 0x00100000);  // first 1MB (boot code, stack, VBE, page dir)
	pd.identity_map_range(PAGE_TABLES_BASE, PAGE_TABLES_BASE + PAGE_TABLE_COUNT * PAGE_SIZE);  // page tables

	// Identity-map framebuffer
	FramebufferInfo *fb_info = (FramebufferInfo *)get_framebuffer_info();
	pd.identity_map_range(fb_info->framebuffer, fb_info->framebuffer + fb_info->size);

	// Identity-map all usable E820 regions
	MemoryMap mmap;
	mmap.init(boot_info->e820_map, boot_info->e820_entry_count);
	mmap.identity_map_usable(pd);

	// Enable paging
	pd.enable();

	// Print memory map for debugging
	mmap.print();

	init_gdt();
}

void platform_interrupts_enable() { asm volatile("sti"); }
void platform_interrupts_disable() { asm volatile("cli"); }
