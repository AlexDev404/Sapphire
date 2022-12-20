#![no_std]
#![no_main]

mod vga_buffer;
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

// static HELLO: &[u8] = b"Hello World!";

#[no_mangle]
// #[link_section = ".text.init"]
pub unsafe extern "C" fn _rust() -> ! {
    // Pixel FMT: x+y*screen_x
    // Setting the unit of the memory to units
    let vga = 0xa0000 as *mut u8;

    for _h in 0..8 {
        // Fill the screen (rainbow)
        for i in 0..0xff {
            fill_screen(vga, 320, 200, i);
            // Beyond the 512K barrier we crash
        }
    }

    // Draw a horizonal line from the center of the screen
    for x in 0..100 {
        *vga.offset(320 / 2 + x + 200 / 2 * 320) = 0x0a;
    }

    // Draw a vertical line from the center of the screen
    for y in 0..100 {
    *vga.offset(320 / 2 + 200 / 2 + y * 320) = 0x0a;
    }

    // Draw a diagonal line from the center of the screen
    for xy in 0..100 {
    *vga.offset(320 / 2 + xy + 200 / 2 + xy * 320) = 0x0a;
    }

    // END

    loop {
    }
}