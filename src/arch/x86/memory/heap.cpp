#include <arch/x86/memory/heap.hpp>

// Global heap instance
static Heap kernel_heap;

void Heap::init(FrameAllocator &fa, PageDirectory &pd, uint32_t num_pages)
{
	// TODO Step 1: Allocate num_pages frames from fa
	// Store the first frame's address as the start of the heap
	// Hint: the first call to alloc_frame() gives you the base address
	uint32_t heap_start = 0;
	// ... your code here ...

	// TODO Step 2: Identity-map the heap region
	// ... your code here ...

	// TODO Step 3: Set total_size
	total_size = 0; // fix this

	// TODO Step 4: Point head at the start of the heap
	head = nullptr; // fix this

	// TODO Step 5: Set up head as one big free block
	// head->size = ???
	// head->free = ???
	// head->next = ???
}

void *Heap::alloc(uint32_t size)
{
	// TODO: Walk the free list and find a block that fits
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

	return nullptr;
}

void Heap::free(void *ptr)
{
	if (!ptr) return;

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
