#include <arch/x86/memory/vmm.hpp>

// Global VMM instance
static VMM kernel_vmm;

void VMM::init(FrameAllocator &frame_alloc, PageDirectory &page_dir)
{
	fa = &frame_alloc;
	pd = &page_dir;
	region_start = VMM_REGION_START;
	total_pages = VMM_TOTAL_PAGES;

	// Allocate physical frames for the bitmap
	uint32_t bitmap_bytes = total_pages / 8;
	uint32_t bitmap_pages = (bitmap_bytes + PAGE_SIZE - 1) / PAGE_SIZE;
	uint32_t bitmap_start = 0;
	for (uint32_t i = 0; i < bitmap_pages; i++)
	{
		uint32_t frame = fa->alloc_frame();
		if (i == 0) bitmap_start = frame;
	}
	pd->identity_map_range(bitmap_start, bitmap_start + bitmap_pages * PAGE_SIZE);
	bitmap = (uint32_t *)bitmap_start;

	// Zero the bitmap (all pages start as free)
	zero_memory(bitmap, bitmap_bytes);
}

uint32_t VMM::find_free_range(uint32_t num_pages)
{
	uint32_t count = 0;
	uint32_t start_index = 0;

	for (uint32_t i = 0; i < total_pages; i++)
	{
		uint32_t idx = i / 32;
		uint32_t bit = i % 32;
		bool used = bitmap[idx] & (1 << bit);

		if (!used)
		{
			if (count == 0) start_index = i;
			count++;
			if (count == num_pages) return start_index;
		}
		else
		{
			count = 0;
		}
	}
	return 0xFFFFFFFF;
}

void VMM::mark_used(uint32_t page_index, uint32_t count)
{
	for (uint32_t i = page_index; i < page_index + count; i++)
	{
		uint32_t idx = i / 32;
		uint32_t bit = i % 32;
		bitmap[idx] |= (1 << bit);
	}
}

void VMM::mark_free(uint32_t page_index, uint32_t count)
{
	for (uint32_t i = page_index; i < page_index + count; i++)
	{
		uint32_t idx = i / 32;
		uint32_t bit = i % 32;
		bitmap[idx] &= ~(1 << bit);
	}
}

void *VMM::alloc_pages(uint32_t num_pages)
{
	uint32_t start_idx = find_free_range(num_pages);
	if (start_idx == 0xFFFFFFFF) return nullptr;

	uint32_t virt_addr = region_start + start_idx * PAGE_SIZE;

	for (uint32_t i = 0; i < num_pages; i++)
	{
		uint32_t frame = fa->alloc_frame();
		pd->map_range(virt_addr + i * PAGE_SIZE, frame, PAGE_SIZE);
	}

	mark_used(start_idx, num_pages);
	return (void *)virt_addr;
}

void VMM::free_pages(void *addr, uint32_t num_pages)
{
	uint32_t page_idx = ((uint32_t)addr - region_start) / PAGE_SIZE;

	for (uint32_t i = 0; i < num_pages; i++)
	{
		uint32_t virt_addr = (uint32_t)addr + i * PAGE_SIZE;
		uint32_t pde_idx = virt_addr >> 22;
		uint32_t pte_idx = (virt_addr >> 12) & 0x3FF;
		uint32_t *page_table = (uint32_t *)(pd->entries[pde_idx] & ~0xFFF);
		uint32_t phys_addr = page_table[pte_idx] & ~0xFFF;

		pd->unmap_range(virt_addr, PAGE_SIZE);
		fa->free_frame(phys_addr);
	}

	mark_free(page_idx, num_pages);
}

// Global wrappers
void *kmalloc_pages(uint32_t num_pages)
{
	return kernel_vmm.alloc_pages(num_pages);
}

void kfree_pages(void *addr, uint32_t num_pages)
{
	kernel_vmm.free_pages(addr, num_pages);
}

void init_vmm(FrameAllocator &fa, PageDirectory &pd)
{
	kernel_vmm.init(fa, pd);
}
