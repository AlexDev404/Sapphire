#include <stdint.h>

// This function runs BEFORE paging is enabled.
// It must be placed in the .boot section so it stays at physical addresses.
// Do NOT use any global variables, string literals, or function calls here
// they would be linked at 0xC0000000+ which doesn't exist yet.

#define PG_DIR 0x9A000 // temporary page directory 

// PDE flags for a 4MB PSE page:
//   bit 0 = Present  (0x01)
//   bit 1 = Writable (0x02)
//   bit 7 = PS — 4MB page (0x80)
#define PSE_4MB_PAGE 0x83

extern "C" __attribute__((section(".boot"))) void bootstrap_paging()
{
	// We set up 4MB PSE pages (no page tables needed, just page directory entries).
	//
	// Page directory index for an address = address >> 22
	//   0x00000000 >> 22 = 0    → PDE[0]
	//   0xC0000000 >> 22 = 768  → PDE[768]

	volatile uint32_t *pd = (volatile uint32_t *)PG_DIR;

	// Step 1: Zero the entire page directory (1024 entries)
	// Simple byte-wise zeroing
	for (uint32_t i = 0; i < 1024; i++)
		pd[i] = 0;
	// Step 2: Enable PSE (Page Size Extension) in CR4
	// PSE lets us use 4MB pages with a single PDE (no page table needed).
	// Hint: read CR4, set bit 4 (0x10), write it back.
	uint32_t cr4;
	asm volatile("mov %%cr4, %0" : "=r"(cr4));
	cr4 |= 0x10;
	asm volatile("mov %0, %%cr4" ::"r"(cr4));

	// Step 3: Identity map first 8MB — PDE[0] and PDE[1]
	// We need 8MB because the page tables live at 0x100000–0x500000 (4MB),
	// and PDE[0] only covers 0x000000–0x3FFFFF. Page tables for higher
	// PDE indices (like PDE[768]) are at 0x400000+, which needs PDE[1].
	pd[0] = 0x00000000 | PSE_4MB_PAGE;
	pd[1] = 0x00400000 | PSE_4MB_PAGE;

	// Step 4: Higher-half map first 8MB — PDE[768] and PDE[769]
	pd[768] = 0x00000000 | PSE_4MB_PAGE;
	pd[769] = 0x00400000 | PSE_4MB_PAGE;

	// Step 5: Load CR3 with the page directory address
	asm volatile("mov %0, %%cr3" ::"r"((uint32_t)(PG_DIR)));

	// Step 6: Enable paging — set bit 31 of CR0
	uint32_t cr0;
	asm volatile("mov %%cr0, %0" : "=r"(cr0));
	cr0 |= 0x80000000;
	asm volatile("mov %0, %%cr0" ::"r"(cr0));
}
