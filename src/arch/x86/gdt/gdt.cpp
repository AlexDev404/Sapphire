#include "gdt.hpp"
#include <driver/debug/debug.hpp>

// The GDT is essentially just a stub - since nobody really uses segmentation like
// they used to anymore, we just set up a flat memory model with a code and data segment
// that both cover the entire 4GB address space. This is sufficient for our needs.
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
	_load_gdt(&g_GDTDescriptor, GDT_CODE_SEGMENT, GDT_DATA_SEGMENT);
	kdebug("GDT: Global Descriptor Table loaded successfully.");
}