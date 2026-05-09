#pragma once
#include <stdint.h>
#include <arch/x86/memory/memory_map.hpp>
#include "constants.hpp"

struct FrameAllocator
{
	uint32_t *bitmap;	   // pointer to the bitmap array
	uint32_t total_frames; // total number of frames tracked
	uint32_t bitmap_size;  // size of bitmap in bytes

	// Initialize the allocator:
	// 1. Find the highest usable address from the memory map to determine total_frames
	// 2. Place the bitmap somewhere in usable memory (after page tables, e.g. 0x500000)
	// 3. Mark ALL frames as used (set every bit to 1)
	// 4. Walk E820 usable regions and mark those frames as free (clear bits)
	// 5. Mark reserved regions back as used:
	//    - First 1MB (0x0 - 0x100000)
	//    - Page tables (0x100000 - 0x500000)
	//    - The bitmap itself
	void init(MemoryMap &mmap);

	// Find the first free frame (first 0 bit), mark it used, return its physical address.
	// Return 0 on failure (out of memory).
	//
	// Hints:
	// - Iterate through bitmap[] entries (each is a uint32_t = 32 frames)
	// - Skip entries that are 0xFFFFFFFF (all used)
	// - When you find one with a free bit, find which bit is 0
	// - The physical address = (array_index * 32 + bit_index) * PAGE_SIZE
	uint32_t alloc_frame();

	// Mark a frame as free (clear its bit).
	//
	// Hints:
	// - Convert address to frame index: index = addr / PAGE_SIZE
	// - Array position: bitmap[index / 32]
	// - Bit position: index % 32
	// - Clear the bit: &= ~(1 << bit)
	void free_frame(uint32_t addr);

private:
	// Mark a single frame as used (set its bit).
	void mark_used(uint32_t addr);

	// Mark a single frame as free (clear its bit).
	void mark_free(uint32_t addr);

	// Mark a range of addresses as used.
	void mark_range_used(uint32_t start, uint32_t end);

	// Mark a range of addresses as free.
	void mark_range_free(uint32_t start, uint32_t end);

	static uint32_t get_frame_number(uint32_t addr)
	{
		return addr / PAGE_SIZE;
	}

	void mark(uint32_t start, uint32_t end, bool _mark_free)
	{
		start &= ~0xFFF;
		end = (end + 0xFFF) & ~0xFFF;

		for (uint32_t addr = start; addr < end && addr >= start; addr += PAGE_SIZE)
		{
			if (_mark_free)
			{
				mark_free(addr);
			}
			else
			{
				mark_used(addr);
			}
		}
	}

	static uint32_t clamp(uint64_t addr)
	{
		return (addr > HIGHEST_USABLE_ADDR)
				   ? HIGHEST_USABLE_ADDR
				   : (uint32_t)addr;
	}
};
