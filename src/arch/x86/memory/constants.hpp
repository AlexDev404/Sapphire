#pragma once

// Bitmap lives right after the page tables
#define BIOS_BASE		0x000000
#define PAGE_BASE		0x100000
#define BITMAP_BASE		0x500000
#define KERNEL_VMA	0xC0000000
#define HIGHEST_USABLE_ADDR 0xFFFFFFFF // 4GB - 1MB (reserved for page tables and bitmap)
