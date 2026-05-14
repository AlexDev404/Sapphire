#include <arch/noarch/platform/platform.hpp>
#include <arch/x86/gdt/gdt.hpp>
#include <arch/x86/idt/idt.hpp>
#include <arch/x86/platform/boot_info.hpp>
#include <arch/x86/fb/fb.h>
#include <arch/x86/paging/paging.hpp>
#include <arch/x86/memory/memory_map.hpp>
#include <arch/x86/memory/frame_alloc.hpp>
#include <arch/x86/memory/heap.hpp>
#include <arch/x86/memory/vmm.hpp>
#include <arch/noarch/syscall/syscall.hpp>
#include <driver/fb/kprint.hpp>
#include <stdint.h>

extern "C" void platform_init(uint32_t boot_info_ptr)
{
	BootInfo *boot_info = reinterpret_cast<BootInfo *>(boot_info_ptr);

	// =========================================================================
	// At this point, stage2 has enabled paging with PSE (4MB pages):
	//   PDE[0]   = identity map:    virtual 0x00000000 → physical 0x00000000 (4MB)
	//   PDE[768] = higher-half map: virtual 0xC0000000 → physical 0x00000000 (4MB)
	//
	// We're running at 0xC000xxxx addresses. The identity map is still active
	// (stage2 code at low addresses needs it). Now we set up proper 4KB page
	// tables and then remove the identity map.
	//
	// NOTE: Don't call kprint/kprintf before pd.enable() — the framebuffer
	// is at a high physical address not covered by the bootstrap 4MB mapping!
	// =========================================================================

	// Set up a NEW page directory at a different physical address.
	// We can't use boot_info->page_directory (0x9C000) because that's the
	// LIVE bootstrap PD in CR3 — pd.init() zeroes it, which would destroy
	// the mappings we're currently running on and triple fault.
	// 0x9D000 is right after it, 4KB aligned, and unused.
	PageDirectory pd;
	pd.init(boot_info->page_directory);

	// Step 1: Map the kernel into the higher half.
	pd.map_range(KERNEL_VMA, 0x00000000, 0x00400000); // 4MB
	// Also keep an identity map of the first 1MB (BIOS data, VGA, etc.):
	pd.identity_map_range(0x00000000, 0x00100000);

	// Step 2: Map the page tables themselves.
	pd.identity_map_range(PAGE_TABLES_BASE, PAGE_TABLES_BASE + PAGE_SIZE * PAGE_TABLE_COUNT);

	// Step 3: Map the framebuffer.
	FramebufferInfo *fb_info = (FramebufferInfo *)get_framebuffer_info();
	pd.identity_map_range(fb_info->framebuffer, fb_info->framebuffer + fb_info->size);

	// Step 4: Map all usable E820 regions (for frame allocator, heap, etc.)
	MemoryMap mmap;
	mmap.init(boot_info->e820_map, boot_info->e820_entry_count);
	mmap.identity_map_usable(pd);

	// Enable the new page tables (replaces stage2's bootstrap PSE mapping)
	pd.enable();

	// NOW we can print — framebuffer is mapped
	kprint(boot_info->e820_entry_count > 0 ? "Memory map found!\n" : "No memory map found!\n");

	// Print memory map for debugging
	mmap.print();

	// Initialize the frame allocator
	FrameAllocator frame_alloc;
	frame_alloc.init(mmap);

	// Initialize the kernel heap (16 pages = 64KB)
	Heap heap;
	heap.init(frame_alloc, pd, 16);

	// Initialize the virtual memory manager
	init_vmm(frame_alloc, pd);

	// Test VMM
	void *p1 = kmalloc_pages(1);
	void *p2 = kmalloc_pages(4);
	kprintf("vmm alloc(1) = 0x%x\n", (uint32_t)p1);
	kprintf("vmm alloc(4) = 0x%x\n", (uint32_t)p2);
	// Write to it to prove it's mapped
	*(uint32_t *)p1 = 0xCAFEBABE;
	kprintf("Wrote 0x%x to p1\n", *(uint32_t *)p1);
	// Free and re-alloc
	kfree_pages(p1, 1);
	void *p3 = kmalloc_pages(1);
	kprintf("Freed p1, re-alloc(1) = 0x%x (should be 0x%x)\n", (uint32_t)p3, (uint32_t)p1);

	// Verify we're running in higher-half — print address of this function
	kprintf("platform_init is at: 0x%x\n", (uint32_t)platform_init);

	gdt::init();
	idt::init();
	interrupts::enable();
	// Syscalls
	syscalls::init();
}
