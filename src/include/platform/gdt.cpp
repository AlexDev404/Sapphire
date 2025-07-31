#include "gdt.hpp"

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

void g_GDTLoad(GDTDescriptor *gdt, uint16_t code_segment_selector, uint16_t data_segment_selector) {
	#ifdef __GNUC__
	asm volatile (
		"lgdt %0\n\t"
		"mov %%ds, %2\n\t"
		"mov %%cs, %1\n\t"
		"mov %%ss, %1\n\t"
		"mov %%es, %1\n\t"
		"mov %%fs, %1\n\t"
		"mov %%gs, %1\n\t"
		"jmp 0x08:1f\n\t" // Far jump to reload CS
		"1:\n\t"
		: // No output
		: "m"(*gdt), "r"(data_segment_selector), "r"(code_segment_selector)
		: "memory", "cc"
	);
	#endif
}

void g_GDTInit() {
	// Load the GDT
	g_GDTLoad(&g_GDTDescriptor, GDT_CODE_SEGMENT, GDT_DATA_SEGMENT);

}