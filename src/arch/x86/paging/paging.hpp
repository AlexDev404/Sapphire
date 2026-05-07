#pragma once
#include <stdint.h>

#define PAGE_SIZE        0x1000
#define PAGE_TABLE_COUNT 1024
#define PAGE_TABLES_BASE 0x100000  // 1MB; 1024 tables * 4KB = 4MB (0x100000..0x500000)

#define PG_PRESENT  0x01
#define PG_WRITE    0x02

struct PageDirectory {
	uint32_t *entries;

	void init(uint32_t *dir_addr);
	void identity_map_range(uint32_t start, uint32_t end);
	void enable();
};
