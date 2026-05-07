#include <arch/x86/paging/paging.hpp>
#include <arch/x86/cr0/cr0.hpp>
#include <arch/x86/cr3/cr3.hpp>

static void zero_memory(void *dest, uint32_t size)
{
	uint8_t *d = (uint8_t *)dest;
	for (uint32_t i = 0; i < size; i++)
		d[i] = 0;
}

void PageDirectory::init(uint32_t *dir_addr)
{
	entries = dir_addr;

	// Zero the page directory and all page tables
	zero_memory(entries, PAGE_SIZE);
	zero_memory((void *)PAGE_TABLES_BASE, PAGE_TABLE_COUNT * PAGE_SIZE);

	// Wire every PDE to its corresponding page table (all present + writable).
	// Individual PTEs default to 0 (not present) unless explicitly mapped.
	for (uint32_t i = 0; i < PAGE_TABLE_COUNT; i++)
	{
		uint32_t pt_addr = PAGE_TABLES_BASE + i * PAGE_SIZE;
		entries[i] = pt_addr | PG_PRESENT | PG_WRITE;
	}
}

void PageDirectory::identity_map_range(uint32_t start, uint32_t end)
{
	// Align start down and end up to page boundaries
	start &= ~0xFFF;
	end = (end + 0xFFF) & ~0xFFF;

	for (uint32_t addr = start; addr < end && addr >= start; addr += PAGE_SIZE)
	{
		uint32_t pde_idx = addr >> 22;
		uint32_t pte_idx = (addr >> 12) & 0x3FF;
		uint32_t *page_table = (uint32_t *)(entries[pde_idx] & ~0xFFF);
		page_table[pte_idx] = addr | PG_PRESENT | PG_WRITE;
	}
}

void PageDirectory::enable()
{
	_write_cr3((uint32_t)entries);
	_write_cr0(_read_cr0() | 0x80000000);
}
