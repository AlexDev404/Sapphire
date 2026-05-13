#pragma once
#include <stdint.h>
#include <arch/x86/memory/frame_alloc.hpp>
#include <arch/x86/paging/paging.hpp>

// Virtual Memory Manager — manages a region of virtual address space.
// Uses a bitmap (1 bit per page) to track which virtual pages are in use.
// Allocates physical frames and maps them on demand.

#define VMM_REGION_START 0xD0000000  // start of managed virtual space
#define VMM_REGION_SIZE  0x20000000  // 512MB
#define VMM_TOTAL_PAGES  (VMM_REGION_SIZE / PAGE_SIZE)  // 131072 pages

struct VMM {
	uint32_t *bitmap;         // 1 bit per virtual page in the managed region
	uint32_t region_start;    // start of managed virtual address space
	uint32_t total_pages;     // number of pages we manage
	FrameAllocator *fa;       // physical frame allocator
	PageDirectory *pd;        // page directory for mapping

	// Initialize the VMM:
	//   1. Store pointers to fa and pd
	//   2. Set region_start and total_pages
	//   3. Allocate physical frames for the bitmap itself
	//      (total_pages / 8 = bytes needed, round up to pages)
	//   4. Identity-map the bitmap frames so we can access them
	//   5. Zero the bitmap (all pages start as free)
	void init(FrameAllocator &fa, PageDirectory &pd);

	// Allocate num_pages contiguous virtual pages, back them with physical frames.
	//
	// Steps:
	//   1. Find num_pages contiguous free bits in the bitmap (use find_free_range)
	//   2. If not found, return nullptr
	//   3. For each page in the range:
	//      a. Allocate a physical frame: fa->alloc_frame()
	//      b. Map it: pd->map_range(virt_addr, phys_addr, PAGE_SIZE)
	//      c. Advance virt_addr by PAGE_SIZE
	//   4. Mark the range as used in the bitmap
	//   5. Return the starting virtual address
	void *alloc_pages(uint32_t num_pages);

	// Free previously allocated virtual pages.
	//
	// Steps:
	//   1. Compute the page index: (addr - region_start) / PAGE_SIZE
	//   2. For each page in the range:
	//      a. Look up the physical address from the page table
	//         (read the PTE: entries[pde_idx] → page_table[pte_idx] & ~0xFFF)
	//      b. Unmap it: pd->unmap_range(virt_addr, PAGE_SIZE)
	//      c. Free the physical frame: fa->free_frame(phys_addr)
	//   3. Mark the range as free in the bitmap
	void free_pages(void *addr, uint32_t num_pages);

private:
	// Find num_pages contiguous free (0) bits in the bitmap.
	// Returns the starting page index, or 0xFFFFFFFF if not found.
	//
	// Algorithm:
	//   Walk the bitmap. For each bit that is 0, count consecutive free bits.
	//   Once count reaches num_pages, return the starting index.
	//   If a 1-bit is found, reset the count and start over.
	uint32_t find_free_range(uint32_t num_pages);

	// Mark 'count' pages starting at page_index as used (set bits to 1)
	void mark_used(uint32_t page_index, uint32_t count);

	// Mark 'count' pages starting at page_index as free (clear bits to 0)
	void mark_free(uint32_t page_index, uint32_t count);
};

// Global wrappers
void *kmalloc_pages(uint32_t num_pages);
void kfree_pages(void *addr, uint32_t num_pages);
void init_vmm(FrameAllocator &fa, PageDirectory &pd);
