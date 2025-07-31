#include "gdt.hpp"
#include <devices/debug/debugcon.h>

GDTEntry g_GDT[] = {
	GDT_ENTRY(0, 0, 0, 0),                                                                                                                                                     // Null descriptor
	GDT_ENTRY(0x00000000, 0xFFFFFFFF, GDT_ACCESS_PRESENT | GDT_ACCESS_RING0 
		| GDT_ACCESS_CODE_SEGMENT | GDT_ACCESS_CODE_READABLE,
		GDT_FLAG_32BIT | GDT_FLAG_GRANULARITY_4K),  // Code segment
	GDT_ENTRY(0x00000000, 0xFFFFFFFF, GDT_ACCESS_PRESENT | GDT_ACCESS_RING0 
		| GDT_ACCESS_DATA_SEGMENT | GDT_ACCESS_DATA_WRITEABLE,
		GDT_FLAG_32BIT | GDT_FLAG_GRANULARITY_4K), // Data segment

	// Add more entries as needed
};

GDTDescriptor g_GDTDescriptor = {
	.limit = sizeof(g_GDT) - 1,
	.base_ptr = g_GDT
};

void g_GDTInit() {
	send_debug("GDT: Initializing Global Descriptor Table...\n");
	// Load the GDT
	g_GDTLoad(&g_GDTDescriptor, GDT_CODE_SEGMENT, GDT_DATA_SEGMENT);
	send_debug("GDT: Global Descriptor Table loaded successfully.\n");

}