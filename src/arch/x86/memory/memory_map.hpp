#pragma once
#include <arch/x86/platform/boot_info.hpp>
#include <arch/x86/paging/paging.hpp>

#define E820_TYPE_USABLE 1

struct MemoryMap {
	E820Entry *entries;
	uint32_t count;

	void init(E820Entry *map, uint32_t entry_count);
	void identity_map_usable(PageDirectory &pd);
	void print();
};
