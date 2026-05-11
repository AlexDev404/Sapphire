#include <arch/noarch/platform/platform.hpp>
#include <arch/x86/gdt/gdt.hpp>
#include <arch/x86/idt/idt.hpp>
#include <arch/x86/platform/boot_info.hpp>
#include <arch/x86/fb/fb.h>
#include <arch/x86/paging/paging.hpp>
#include <arch/x86/memory/memory_map.hpp>
#include <arch/x86/memory/frame_alloc.hpp>
#include <arch/x86/memory/heap.hpp>
#include <driver/fb/kprint.hpp>
#include <stdint.h>

extern "C" void platform_init(uint32_t boot_info_ptr)
{
	BootInfo *boot_info = reinterpret_cast<BootInfo *>(boot_info_ptr);
	kprint(boot_info->e820_entry_count > 0 ? "Memory map found!\n" : "No memory map found!\n");

	// Set up paging
	PageDirectory pd;
	pd.init(boot_info->page_directory);

	// Identity-map critical regions
	pd.identity_map_range(0x00000000, 0x00100000);  // first 1MB (boot code, stack, VBE, page dir)
	pd.identity_map_range(PAGE_TABLES_BASE, PAGE_TABLES_BASE + PAGE_TABLE_COUNT * PAGE_SIZE);  // page tables

	// Identity-map framebuffer
	FramebufferInfo *fb_info = (FramebufferInfo *)get_framebuffer_info();
	pd.identity_map_range(fb_info->framebuffer, fb_info->framebuffer + fb_info->size);

	// Identity-map all usable E820 regions
	MemoryMap mmap;
	mmap.init(boot_info->e820_map, boot_info->e820_entry_count);
	mmap.identity_map_usable(pd);

	// Enable paging
	pd.enable();

	// Print memory map for debugging
	mmap.print();

	// Initialize the frame allocator
	FrameAllocator frame_alloc;
	frame_alloc.init(mmap);

	// Initialize the kernel heap (16 pages = 64KB)
	Heap heap;
	heap.init(frame_alloc, pd, 16);

	// Test: kmalloc/kfree
	uint32_t *a = (uint32_t *)heap.alloc(100);
	uint32_t *b = (uint32_t *)heap.alloc(200);
	uint32_t *c = (uint32_t *)heap.alloc(50);
	kprintf("alloc(100) = 0x%x\n", (uint32_t)a);
	kprintf("alloc(200) = 0x%x\n", (uint32_t)b);
	kprintf("alloc(50)  = 0x%x\n", (uint32_t)c);

	// Write to allocated memory to prove it works
	*a = 0xDEADBEEF;
	kprintf("Wrote 0x%x to a\n", *a);

	// Free b and re-allocate — should reuse b's address
	heap.free(b);
	uint32_t *d = (uint32_t *)heap.alloc(200);
	kprintf("Freed b, re-alloc(200) = 0x%x (should be 0x%x)\n", (uint32_t)d, (uint32_t)b);

	init_gdt();
	init_idt();
}

void platform_interrupts_enable() { asm volatile("sti"); }
void platform_interrupts_disable() { asm volatile("cli"); }
