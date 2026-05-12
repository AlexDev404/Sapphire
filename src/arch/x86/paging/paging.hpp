#pragma once
#include <stdint.h>

#define PAGE_SIZE        0x1000
#define PAGE_TABLE_COUNT 1024
#define PAGE_TABLES_BASE 0x100000  // 1MB; 1024 tables * 4KB = 4MB (0x100000..0x500000)

#define PG_NOTPRESENT	0x00
#define PG_PRESENT		0x01
#define PG_WRITE		0x02

struct PageDirectory {
	uint32_t *entries;

	void init(uint32_t *dir_addr);
	void identity_map_range(uint32_t start, uint32_t end);

	// Map a virtual address range to a different physical address range.
	// Example: map_range(0xC0000000, 0x00000000, 0x00400000)
	//   maps virtual 0xC0000000..0xC03FFFFF → physical 0x00000000..0x003FFFFF
	void map_range(uint32_t virt_start, uint32_t phys_start, uint32_t size);

	// Unmap a virtual address range (set PTEs to 0 / not present).
	// Used to remove the identity map after switching to higher-half.
	void unmap_range(uint32_t virt_start, uint32_t size);

	void enable();
};
