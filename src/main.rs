#![no_std]
#![no_main]
extern crate lazy_static;

mod graphics;
mod types;
use crate::types::graphics::VbeModeInfo;
use core::panic::PanicInfo;
use tinyvec_string::ArrayString;
use crate::graphics::putpixel;
use crate::graphics::print_string as x_string;
use crate::graphics::drawchar;
static mut VBE_DATA: *const VbeModeInfo = 0 as *const _;

use crate::graphics::fonts::ibm_vga8x16;

static F_DATA: [u8; 4096] = ibm_vga8x16::IBM_VGA_8X16;
static F_HEIGHT: isize = 16; // Rows
static F_WIDTH: isize = 8; // Columns

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

pub fn fill_screen(vbe_data: &VbeModeInfo, color: u8) {
    let screen_x: isize = vbe_data.width as isize;
    let screen_y: isize = vbe_data.height as isize;
    let framebuffer = vbe_data.framebuffer as *mut u8;

    for y in 0..screen_y {
        for x in 0..screen_x {
            unsafe {
                let offset = y * (vbe_data.pitch as isize) + x * ((vbe_data.bpp as isize) / 8);
                *framebuffer.wrapping_offset(offset as isize) = color;
            }
        }
    }
}

// This function is called on panic.
#[panic_handler]
fn panic(_info: &PanicInfo) -> ! {
    unsafe {
        drawchar('E', 0, 0, 0x4, 0x0, &*VBE_DATA);
    }
    loop {
    }
}

#[no_mangle]
// #[link_section = ".text.init"]
pub unsafe extern "C" fn _rust(vbe_mode_info: *const VbeModeInfo) -> ! {
    // Works
    /****************************************** */
    let vbe_data = &*vbe_mode_info;
    VBE_DATA = vbe_mode_info;
    // fill_screen(vbe_data, 0x2);
    // for _h in 0..8 {
        // Fill the screen (rainbow)
        // for i in 0..0xff {
        //     fill_screen(vbe_data, i);
        //     // Beyond the 512K barrier we crash
        // }
    // }
    // putpixel(vbe_data, 0x2, 0, 0);
    // putpixel(vbe_data, 0x2, 1, 0);
    // putpixel(vbe_data, 0x2, 2, 0);
    // putpixel(vbe_data, 0x2, 3, 0);
    // putpixel(vbe_data, 0x2, 4, 0);
    drawchar('A', 30, 30, 0x0a, 0x00, &*VBE_DATA);
    drawchar('B', 38, 30, 0x0a, 0x00, vbe_data);
    drawchar('C', 46, 30, 0x0a, 0x00, vbe_data);
    // drawchar('A', 40, 30, 0x0a, 0x00, &*VBE_DATA);
    // drawchar('B', 48, 30, 0x0a, 0x00, vbe_data);
    // drawchar('C', 56, 30, 0x0a, 0x00, vbe_data);
    // print_string(ArrayString::<[u8; 10]>::from("HE"), 0x0f, 0x00, 50, 50, vbe_data);
    // x_string(ArrayString::<[u8; 10]>::from("HE"), 0x0f, 0x00, 70, 50, vbe_data); // <--- This code is giving trouble @todo Fix.

    // END
    loop {
    }
}
