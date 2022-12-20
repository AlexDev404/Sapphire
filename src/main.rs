#![no_std]
#![no_main]

use core::panic::PanicInfo;

// This function is called on panic.
#[panic_handler]
fn panic(_info: &PanicInfo) -> ! {
    loop {
    }
}

// fn fill_screen(vga: *mut u8, screen_x: isize, screen_y: isize, color: u8) {
//     for x in 0..screen_x {
//         for y in 0..screen_y {
//             unsafe {
//                 *vga.offset(x + y * 320) = color;
//             }
//         }
//     }
// }

static HELLO: &[u8] = b"Hello World!";

#[no_mangle]
// #[link_section = ".text.init"]
pub unsafe extern "C" fn _rust() -> ! {

    let vga_buffer = 0xb8000 as *mut u8;

    for (i, &byte) in HELLO.iter().enumerate() {
        unsafe {
            *vga_buffer.offset(i as isize * 2) = byte;
            *vga_buffer.offset(i as isize * 2 + 1) = 0xb;
        }
    }
    loop {
    }
}