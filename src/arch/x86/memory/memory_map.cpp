#include <arch/x86/memory/memory_map.hpp>
#include <driver/fb/kprint.hpp>

void MemoryMap::init(E820Entry *map, uint32_t entry_count)
{
	entries = map;
	count = entry_count;
}

void MemoryMap::identity_map_usable(PageDirectory &pd)
{
	for (uint32_t i = 0; i < count; i++)
	{
		E820Entry *entry = &entries[i];
		if (entry->type != E820_TYPE_USABLE)
			continue;

		// Clamp to 32-bit address space
		uint32_t base = (uint32_t)entry->base;
		uint64_t end64 = entry->base + entry->length;
		uint32_t end = (end64 > 0xFFFFFFFF) ? 0xFFFFFFFF : (uint32_t)end64;

		pd.identity_map_range(base, end);
	}
}

void MemoryMap::print()
{
	kprintf("Memory map entry count: %d\n", count);
	for (uint32_t i = 0; i < count; i++)
	{
		E820Entry *entry = &entries[i];
		kprintf("Entry %d: Base=0x%x, Length=0x%x, Type=%d\n", i + 1,
				(uint32_t)(entry->base), (uint32_t)(entry->length), (uint32_t)(entry->type));
	}
}
