#include <arch/x86/memory/heap.hpp>

// Global heap instance
static Heap kernel_heap;

void Heap::init(FrameAllocator &fa, PageDirectory &pd, uint32_t num_pages)
{
	// Step 1: Allocate num_pages frames from fa
	// Store the first frame's address as the start of the heap
	// Hint: the first call to alloc_frame() gives you the base address

	uint32_t heap_start = 0;
	uint32_t heap_end = 0;
	for (uint32_t i = 0; i < num_pages; i++)
	{
		heap_end = fa.alloc_frame();
		if (i == 0)
			heap_start = heap_end;
	}

	// Step 2: Identity-map the heap region
	pd.identity_map_range(heap_start, heap_end + PAGE_SIZE);

	// Step 3: Set total_size
	total_size = (heap_end + PAGE_SIZE) - heap_start;

	// Step 4: Point head at the start of the heap
	head = (BlockHeader *)(heap_start);

	// Step 5: Set up head as one big free block
	head->size = total_size - sizeof(BlockHeader);
	head->free = true;
	head->next = nullptr;
}

void *Heap::alloc(uint32_t size)
{
	// Walk the free list and find a block that fits
	// Hint: start at head, follow ->next
	//
	// For each block:
	//   if (!block->free || block->size < size) continue;
	//
	//   If block is big enough to split
	//   (block->size >= size + sizeof(BlockHeader) + MIN_SPLIT):
	//     - Create new header at: (uint8_t *)block + sizeof(BlockHeader) + size
	//     - New block: size = leftover, free = true, next = block->next
	//     - Current block: size = size, next = new block
	//
	//   Mark block->free = false
	//   Return (void *)(block + 1)
	//
	// If nothing found, return nullptr

	// Walk the linked list and find a block that fits

	// Start at head
	BlockHeader *block = head;
	while (block)
	{
		if (!block->free || block->size < size)
		{
			block = block->next;
			continue;
		}
		// If the block is big enough to split create a new header
		if (block->size >= size + sizeof(BlockHeader) + MIN_SPLIT)
		{
			// Carve out space from this block to fit the amount of space we need
			BlockHeader *newBlock = (BlockHeader *)((uint8_t *)block + sizeof(BlockHeader) + size);
			newBlock->free = true;
			// .. Transfer the old pointer to newBlock
			newBlock->next = block->next;
			// Update new block size to the leftover space
			newBlock->size = ((block->size) - size) - sizeof(BlockHeader);
			// Resize block to the space we're allocating
			block->size = size;
			// Update block to now be used
			block->free = false;
			// Block now gets set to newBlock
			block->next = newBlock;

			// The chain is now complete.

			// Return the block address after the header
			return (void *)(block + 1);
		}

		// If the block is to small to split, but is big enough
		// Just use it directly
		block->free = false;
		return (void *)(block + 1);
	}
	// If nothing is found, return nullptr
	return nullptr;
}

void Heap::free(void *ptr)
{
	if (!ptr)
		return;

	// TODO Step 1: Get the block header
	// Hint: BlockHeader *block = (BlockHeader *)ptr - 1;
	// (ptr points to the usable area, the header is right before it)

	// TODO Step 2: Mark it free
	// block->free = true;

	// TODO Step 3: Merge adjacent free blocks
	// Walk from head. For each block:
	//   while (block->free && block->next && block->next->free):
	//     block->size += sizeof(BlockHeader) + block->next->size
	//     block->next = block->next->next
}

// Global wrappers
void *kmalloc(uint32_t size)
{
	return kernel_heap.alloc(size);
}

void kfree(void *ptr)
{
	kernel_heap.free(ptr);
}
