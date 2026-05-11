#pragma once
#include <stdint.h>
#include <arch/x86/memory/frame_alloc.hpp>
#include <arch/x86/paging/paging.hpp>

struct BlockHeader {
	uint32_t size;       // size of usable area (NOT including this header)
	bool free;           // is this block available?
	BlockHeader *next;   // next block in the list (contiguous in memory)
};

struct Heap {
	BlockHeader *head;     // first block in the list
	uint32_t total_size;   // total heap size in bytes

	// Allocate num_pages from the frame allocator, identity-map them,
	// and set up one big free block spanning the entire region.
	//
	// Steps:
	// 1. Allocate num_pages contiguous frames from fa
	//    (call alloc_frame() in a loop — they should be contiguous since
	//     we haven't fragmented anything yet)
	// 2. Identity-map them using pd.identity_map_range(start, end)
	// 3. total_size = num_pages * PAGE_SIZE
	// 4. head = (BlockHeader *) start of the allocated region
	// 5. Set up head as one big free block:
	//    - size = total_size - sizeof(BlockHeader)
	//    - free = true
	//    - next = nullptr
	void init(FrameAllocator &fa, PageDirectory &pd, uint32_t num_pages);

	// Find a free block large enough for 'size' bytes, mark it used,
	// and return a pointer to the usable memory (just past the header).
	//
	// Steps:
	// 1. Walk the list starting at head
	// 2. For each block, check: is it free AND is block->size >= size?
	// 3. If the block is much bigger (block->size >= size + sizeof(BlockHeader) + MIN_SPLIT),
	//    split it:
	//    a. Create a new BlockHeader at (block + sizeof(BlockHeader) + size)
	//    b. New block gets the leftover size, free = true, next = block->next
	//    c. Current block's size = size, next = new block
	// 4. Mark current block as free = false
	// 5. Return pointer to (block + 1)  — i.e., the memory right after the header
	// 6. If no block found, return nullptr
	void *alloc(uint32_t size);

	// Free a previously allocated block and merge adjacent free blocks.
	//
	// Steps:
	// 1. Get the header: BlockHeader *block = (BlockHeader *)ptr - 1
	// 2. Set block->free = true
	// 3. Merge pass: walk the entire list from head.
	//    For each block, if it's free AND block->next is also free:
	//    - Absorb the next block: block->size += sizeof(BlockHeader) + block->next->size
	//    - Skip over it: block->next = block->next->next
	void free(void *ptr);
};

// Minimum bytes of usable space to justify splitting a block
#define MIN_SPLIT 16

// Global wrappers (implement in heap.cpp)
void *kmalloc(uint32_t size);
void kfree(void *ptr);
