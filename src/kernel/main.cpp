#include <driver/fb/fb.hpp>
#include <arch/noarch/debug/debug.hpp>

extern "C" void kmain(){
    debug("Hello, World! This is a test of the kernel's main function.");
    putpixel(100, 100, 0xFF0000);
}