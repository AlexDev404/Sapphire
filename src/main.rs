#![no_std]
#![no_main]
extern crate lazy_static;

mod graphics;
mod types;
use crate::types::graphics::VbeModeInfo;
use core::panic::PanicInfo;
use tinyvec_string::ArrayString;
use crate::graphics::print_string as x_string;
use crate::graphics::drawchar;

pub fn print_string(
    str: ArrayString<[u8; 10]>,
    fgcolor: u8,
    bgcolor: u8,
    start_x: isize,
    y: isize,
    vbe_data: &VbeModeInfo
) {
    // Stack is max 13?? Why?
    let mut pos: isize = start_x;
    for chr in str.chars() {
        drawchar(chr, pos, y, fgcolor, bgcolor, vbe_data);
        // drawchar(chr, pos, y, fgcolor, bgcolor, vbe_data);
        pos += 9; // Each character is 8 bytes wide and we need at least 1 byte of separation
    }
}

// This function is called on panic.
#[panic_handler]
fn panic(_info: &PanicInfo) -> ! {
    loop {
    }
}

#[no_mangle]
// #[link_section = ".text.init"]
pub unsafe extern "C" fn _rust(vbe_mode_info: *const VbeModeInfo) -> ! {
    // Works
    /****************************************** */
    let vbe_data = &*vbe_mode_info;
    drawchar('A', 30, 30, 0x0a, 0x00, vbe_data);
    // drawchar('B', 38, 30, 0x0a, 0x00, vbe_data);
    // drawchar('C', 46, 30, 0x0a, 0x00, vbe_data);
    print_string(ArrayString::<[u8; 10]>::from("HE"), 0x0f, 0x00, 50, 50, vbe_data);
    x_string(ArrayString::<[u8; 10]>::from("HE"), 0x0f, 0x00, 70, 50, vbe_data); // <--- This code is giving trouble @todo Fix.

    // END
    loop {
    }
}
