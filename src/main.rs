#![no_std]
#![no_main]

mod graphics;
use core::panic::PanicInfo;
use graphics::putpixel;

// This function is called on panic.
#[panic_handler]
fn panic(_info: &PanicInfo) -> ! {
    loop {
    }
}

// static HELLO: &[u8] = b"Hello World!";

#[no_mangle]
// #[link_section = ".text.init"]
pub unsafe extern "C" fn _rust() -> ! {
    // Pixel FMT: x+y*screen_x
    // Setting the unit of the memory to units
    let vga = 0xa0000 as *mut u8;

putpixel(vga, 0x0a, 0, 0);
    // END

    loop {
    }
}