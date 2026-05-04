#include "gdt.hpp"
#include <driver/debug/debug.hpp>

GDTEntry g_GDT[] = {
	GDT_ENTRY(0, 0, 0, 0),                                          // Null descriptor
	GDT_ENTRY(0x00000000, 0xFFFFFFFF,                               // Code segment
		GDT_ACCESS_PRESENT | GDT_ACCESS_RING0
		| GDT_ACCESS_CODE_SEGMENT | GDT_ACCESS_CODE_READABLE,
		GDT_FLAG_32BIT | GDT_FLAG_GRANULARITY_4K),
	GDT_ENTRY(0x00000000, 0xFFFFFFFF,                               // Data segment
		GDT_ACCESS_PRESENT | GDT_ACCESS_RING0
		| GDT_ACCESS_DATA_SEGMENT | GDT_ACCESS_DATA_WRITEABLE,
		GDT_FLAG_32BIT | GDT_FLAG_GRANULARITY_4K),

	// Add more entries as needed (TSS, ring 3 code/data, etc.)
};

GDTDescriptor g_GDTDescriptor = {
	.limit = sizeof(g_GDT) - 1,
	.base  = (uint32_t)g_GDT
};

void init_gdt() {
	kdebug("GDT: Initializing Global Descriptor Table...");
	load_gdt(&g_GDTDescriptor, GDT_CODE_SEGMENT, GDT_DATA_SEGMENT);
	kdebug("GDT: Global Descriptor Table loaded successfully.");
}