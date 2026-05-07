#include <arch/noarch/platform/platform.hpp>
#include <arch/x86/gdt/gdt.hpp>
#include <arch/x86/platform/boot_info.hpp>
#include <arch/x86/fb/fb.h>
#include <arch/x86/cr0/cr0.hpp>
#include <arch/x86/cr3/cr3.hpp>
#include <driver/fb/kprint.hpp>
#include <stdint.h>

// Page tables live at 0x100000 (1MB). 1024 tables * 4KB = 4MB total (0x100000..0x500000).
#define PAGE_TABLES_BASE 0x100000
#define PAGE_TABLE_COUNT 1024
#define PAGE_SIZE        0x1000

// Page entry flags
#define PG_PRESENT  0x01
#define PG_WRITE    0x02

static void zero_memory(void *dest, uint32_t size)
{
	uint8_t *d = (uint8_t *)dest;
	for (uint32_t i = 0; i < size; i++)
		d[i] = 0;
}

static void identity_map_range(uint32_t *page_dir, uint32_t start, uint32_t end)
{
	// Align start down and end up to page boundaries
	start &= ~0xFFF;
	end = (end + 0xFFF) & ~0xFFF;

	for (uint32_t addr = start; addr < end && addr >= start; addr += PAGE_SIZE)
	{
		uint32_t pde_idx = addr >> 22;
		uint32_t pte_idx = (addr >> 12) & 0x3FF;
		uint32_t *page_table = (uint32_t *)(page_dir[pde_idx] & ~0xFFF);
		page_table[pte_idx] = addr | PG_PRESENT | PG_WRITE;
	}
}

extern "C" void platform_init(uint32_t boot_info_ptr)
{
	BootInfo *boot_info = reinterpret_cast<BootInfo *>(boot_info_ptr);
	kprint(boot_info->e820_entry_count > 0 ? "Memory map found!\n" : "No memory map found!\n");

	uint32_t *page_dir = boot_info->page_directory;

	// Zero the page directory and all page tables
	zero_memory(page_dir, PAGE_SIZE);
	zero_memory((void *)PAGE_TABLES_BASE, PAGE_TABLE_COUNT * PAGE_SIZE);

	// Wire every PDE to its corresponding page table (all present + writable).
	// Individual PTEs default to 0 (not present) unless explicitly mapped below.
	for (uint32_t i = 0; i < PAGE_TABLE_COUNT; i++)
	{
		uint32_t pt_addr = PAGE_TABLES_BASE + i * PAGE_SIZE;
		page_dir[i] = pt_addr | PG_PRESENT | PG_WRITE;
	}

	// Always identity-map the first 1MB (boot code, stack, VBE data, page directory)
	identity_map_range(page_dir, 0x00000000, 0x00100000);

	// Identity map the framebuffer address
	FramebufferInfo *fb_info = (FramebufferInfo *)get_framebuffer_info();
	uint32_t fb_addr = fb_info->framebuffer;
	uint32_t fb_size = fb_info->size;

	identity_map_range(page_dir, fb_addr, fb_addr + fb_size);
	
	// Always identity-map the page tables themselves so they remain accessible
	identity_map_range(page_dir, PAGE_TABLES_BASE, PAGE_TABLES_BASE + PAGE_TABLE_COUNT * PAGE_SIZE);

	// Walk E820 and identity-map all usable (type==1) regions
	for (uint32_t i = 0; i < boot_info->e820_entry_count; i++)
	{
		E820Entry *entry = &boot_info->e820_map[i];
		if (entry->type != 1)
			continue;

		// Clamp to 32-bit address space
		uint32_t base = (uint32_t)entry->base;
		uint64_t end64 = entry->base + entry->length;
		uint32_t end = (end64 > 0xFFFFFFFF) ? 0xFFFFFFFF : (uint32_t)end64;

		identity_map_range(page_dir, base, end);
	}

	// Load page directory into CR3 and enable paging
	_write_cr3((uint32_t)page_dir);
	_write_cr0(_read_cr0() | 0x80000000);

	// Print memory map for debugging
	kprintf("Memory map entry count: %d\n", boot_info->e820_entry_count);
	for (uint32_t i = 0; i < boot_info->e820_entry_count; i++)
	{
		E820Entry *entry = &boot_info->e820_map[i];
		kprintf("Entry %d: Base=0x%x, Length=0x%x, Type=%d\n", i + 1,
				(uint32_t)(entry->base), (uint32_t)(entry->length), (uint32_t)(entry->type));
	}

	init_gdt();
}

void platform_interrupts_enable() { asm volatile("sti"); }
void platform_interrupts_disable() { asm volatile("cli"); }
