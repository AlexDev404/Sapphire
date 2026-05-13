#include <arch/x86/paging/paging.hpp>
#include <arch/x86/cr0/cr0.hpp>
#include <arch/x86/cr3/cr3.hpp>
#include <arch/x86/cr4/cr4.hpp>

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
	// Write the entries and turn on paging
	_write_cr3((uint32_t)entries);
	_write_cr0(_read_cr0() | 0x80000000);
	// Disable 4MB paging
	_write_cr4(_read_cr4() & ~0x10);
}

// Map virtual address range → physical address range using 4KB pages.
// Similar to identity_map_range, but virtual and physical don't have to match.
//
// This function.
// Steps:
//   1. Align virt_start and phys_start down to page boundaries
//   2. Align size up to page boundary
//   3. Loop: for each page in the range:
//      a. Compute pde_idx = virt_addr >> 22
//      b. Compute pte_idx = (virt_addr >> 12) & 0x3FF
//      c. Get the page table pointer from entries[pde_idx]: (entries[pde_idx] & ~0xFFF)
//      d. Set page_table[pte_idx] = phys_addr | PG_PRESENT | PG_WRITE
//      e. Advance both virt_addr and phys_addr by PAGE_SIZE
//
// Hint: It's almost identical to identity_map_range, except you track
// virt_addr and phys_addr separately instead of using one address for both.
void PageDirectory::map_range(uint32_t virt_start, uint32_t phys_start, uint32_t size)
{
	// Align start down and end up to page boundaries
	virt_start &= ~0xFFF;
	phys_start &= ~0xFFF;
	size = (size + 0xFFF) & ~0xFFF;

	const uint32_t virt_end = virt_start + size;
	while (virt_start < virt_end)
	{
		uint32_t pde_idx = virt_start >> 22;
		uint32_t pte_idx = (virt_start >> 12) & 0x3FF;
		uint32_t *page_table = (uint32_t *)(entries[pde_idx] & ~0xFFF);
		page_table[pte_idx] = phys_start | PG_PRESENT | PG_WRITE;

		phys_start += PAGE_SIZE;
		virt_start += PAGE_SIZE;
	}
}

// Unmap a virtual address range — set PTEs to 0 (not present).
// Used to remove the bootstrap identity map once we're running in higher-half.
//
// Implement this function.
// Steps:
//   1. Align virt_start down, compute end from size (align up)
//   2. Loop: for each page:
//      a. Compute pde_idx and pte_idx from virt_addr
//      b. Get page table pointer from entries[pde_idx]
//      c. Set page_table[pte_idx] = 0
//      d. Advance virt_addr by PAGE_SIZE
//   3. Flush the TLB after unmapping by reloading CR3:
//      _write_cr3((uint32_t)entries);
//
// Hint: Same loop structure as map_range, just write 0 instead of an address.
void PageDirectory::unmap_range(uint32_t virt_start, uint32_t size)
{
	// Align start down and end up to page boundaries
	virt_start &= ~0xFFF;
	size = (size + 0xFFF) & ~0xFFF;

	const uint32_t virt_end = virt_start + size;
	while (virt_start < virt_end)
	{
		uint32_t pde_idx = virt_start >> 22;
		uint32_t pte_idx = (virt_start >> 12) & 0x3FF;
		uint32_t *page_table = (uint32_t *)(entries[pde_idx] & ~0xFFF);
		page_table[pte_idx] = PG_NOTPRESENT;

		virt_start += PAGE_SIZE;
	}
	// Flush cache
	_write_cr3((uint32_t)entries);
}
