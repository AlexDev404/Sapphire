#![no_std]
#![no_main]

mod graphics;
use core::panic::PanicInfo;

// This function is called on panic.
#[panic_handler]
fn panic(_info: &PanicInfo) -> ! {
    loop {
    }
}

fn fill_screen(vga: *mut u8, screen_x: isize, screen_y: isize, color: u8) {
    for x in 0..screen_x {
        for y in 0..screen_y {
            unsafe {
                *vga.offset(x + y * 320) = color;
            }
        }
    }
}

fn putpixel(vga: *mut u8, color: u8, x: isize, y: isize) {
    unsafe {
        *vga.offset(x + y * 320) = color;
    }
}

// static HELLO: &[u8] = b"Hello World!";

#[no_mangle]
// #[link_section = ".text.init"]
pub unsafe extern "C" fn _rust() -> ! {
    // Pixel FMT: x+y*screen_x
    // Setting the unit of the memory to units
    // let vga = 0xa0000 as *mut u8;


    // END

    loop {
    }
}