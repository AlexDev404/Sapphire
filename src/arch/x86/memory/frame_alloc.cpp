#include <arch/x86/memory/frame_alloc.hpp>
#include <arch/x86/paging/paging.hpp>
#include <stdint.h>

void FrameAllocator::init(MemoryMap &mmap)
{
	// Step 1: Find the highest physical address from E820 usable entries
	// Loop through mmap.entries[0..count-1], track max of (base + length)
	// Clamp to HIGHEST_USABLE_ADDR for 32-bit
	uint32_t highest_addr = 0;
	for (uint32_t i = 0; i < mmap.count; i++)
	{
		E820Entry entry = mmap.entries[i];
		// Skip non-free entries
		if (entry.type != 1)
			continue;

		uint64_t end64 = entry.base + entry.length;
		uint32_t end32 = clamp(end64);

		// Clamp to HIGHEST_USABLE_ADDR and track max
		if (end32 > highest_addr)
		{
			highest_addr = end32;
		}
	}

	// Step 2: Calculate total_frames and bitmap_size
	total_frames = highest_addr / PAGE_SIZE;
	bitmap_size = (total_frames + 7) / 8; // round up to nearest byte

	// Step 3: Place the bitmap at BITMAP_BASE
	bitmap = (uint32_t *)BITMAP_BASE;

	// Step 4: Set all bits to 1 (mark everything used)
	// fill bitmap with 0xFF bytes (each bit = 1 = used)
	uint8_t *b_ptr = (uint8_t *)bitmap;
	for (uint32_t i = 0; i < bitmap_size; i++)
	{
		b_ptr[i] = 0xFF;
	}

	// Step 5: Walk E820 usable regions → mark those frames FREE
	// Hint: use mark_range_free() for each usable entry
	for (uint32_t i = 0; i < mmap.count; i++)
	{
		E820Entry entry = mmap.entries[i];
		// Skip non-free entries
		if (entry.type != 1)
			continue;

		mark_range_free(entry.base, clamp(entry.base + entry.length));
	}

	// Step 6: Mark critical regions back as USED so they never get allocated:
	// - First 1MB:        0x000000 - 0x100000
	// - Page tables:      0x100000 - 0x500000
	// - Bitmap itself:    BITMAP_BASE to BITMAP_BASE + bitmap_size
	mark_range_used(BIOS_BASE, PAGE_BASE);
	mark_range_used(PAGE_BASE, BITMAP_BASE);
	mark_range_used(BITMAP_BASE, BITMAP_BASE + bitmap_size);
}

uint32_t FrameAllocator::alloc_frame()
{
	// Find first free frame
	// Procedure: iterate bitmap[] in units of uint32_t
	//   - How many uint32_t entries? bitmap_size / sizeof(uint32_t)
	//   - Skip if bitmap[i] == HIGHEST_USABLE_ADDR
	//   - Otherwise, find which bit j (0-31) is 0
	//   - Set that bit to 1
	//   - Return physical address: (i * 32 + j) * PAGE_SIZE
	for (uint32_t i = 0; i < bitmap_size / sizeof(uint32_t); i++)
	{
		// if all bits are used
		if (bitmap[i] == 0xFFFFFFFF)
			continue;

		// Found an entry with a free bit, find which bit is 0
		for (uint32_t j = 0; j < 32; j++)
		{
			if ((bitmap[i] & (1 << j)) == 0) // if bit j is free
			{
				bitmap[i] |= (1 << j);			   // mark it as used
				return ((i * 32 + j) * PAGE_SIZE); // return physical address
			}
		}
	}

	return 0; // 0 = out of memory
}

void FrameAllocator::free_frame(uint32_t addr)
{
	// Clear the bit for this frame
	mark_free(addr);
}

void FrameAllocator::mark_used(uint32_t addr)
{
	uint32_t frameNumber = get_frame_number(addr);
	// 32 because each uint"32"_t in the bitmap tracks 32 frames (1 bit per frame)
	uint32_t bitmapIndex = frameNumber / 32;
	uint32_t bitPosition = frameNumber % 32;
	// example: b11110111 OR b00001000 = b11111111 (bit 3 set)
	bitmap[bitmapIndex] |= (1 << bitPosition);
}

void FrameAllocator::mark_free(uint32_t addr)
{
	uint32_t frameNumber = get_frame_number(addr);
	uint32_t bitmapIndex = frameNumber / 32;
	uint32_t bitPosition = frameNumber % 32;
	bitmap[bitmapIndex] &= ~(1 << bitPosition);
}

void FrameAllocator::mark_range_used(uint32_t start, uint32_t end)
{
	// Align start down, end up to PAGE_SIZE, then mark each frame
	mark(start, end, false);
}

void FrameAllocator::mark_range_free(uint32_t start, uint32_t end)
{
	// Same as above but call mark_free for each frame
	mark(start, end, true);
}
