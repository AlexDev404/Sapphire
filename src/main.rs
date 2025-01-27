#![no_std]
#![no_main]
extern crate lazy_static;

mod graphics;
mod types;
use crate::types::graphics::VbeModeInfo;
use core::panic::PanicInfo;
use core::arch::asm;
use core::ptr;
use crate::graphics::putpixel;
use crate::graphics::print_string as x_string;
use crate::graphics::drawchar;
// use crate::graphics::drawchar;
static mut VBE_DATA: *const VbeModeInfo = 0 as *const _;

use crate::graphics::fonts::ibm_vga8x16;

static FONT_DATA: [u8; 4096] = ibm_vga8x16::IBM_VGA_8X16;
static FONT_HEIGHT: isize = 16; // Rows
static FONT_WIDTH: isize = 8; // Columns

// #[no_mangle]
// pub fn drawchar(chr: char, x: isize, y: isize, fgcolor: u64, bgcolor: u64, vbe_data: &VbeModeInfo) {
//     // Convert character to u8 and ensure it's within bounds
//     let c = chr as u8;
//     if (c as usize) >= FONT_DATA.len() / (FONT_HEIGHT as usize) {
//         return; // Handle out-of-bounds character
//     }

//     // Calculate the start index of the glyph in the FONT_DATA array
//     let start_index = (c as usize) * (FONT_HEIGHT as usize);
//     let glyph = &FONT_DATA[start_index..start_index + (c as usize) * (FONT_HEIGHT as usize)];
//     unsafe {
//         let glyph02: u8 = ptr::addr_of!(glyph) as u8;
//         asm!("mov dx, 0xE9");
//         asm!("mov al, {}", in(reg_byte) ('G' as u8));
//         asm!("out dx, al");
//         asm!("mov al, {}", in(reg_byte) (glyph02 as u8));
//         asm!("out dx, al");
//     }

//     const MASK_BASE: u8 = 0x80;

//     for cy in 0..FONT_HEIGHT {
//         let glyph_byte = glyph[cy as usize];
//         unsafe {
//             asm!("mov dx, 0xE9");
//             asm!("mov al, {}", in(reg_byte) ('B' as u8));
//             asm!("out dx, al");
//             asm!("mov al, {}", in(reg_byte) (glyph_byte as u8));
//             asm!("out dx, al");
//         }
//         for cx in 0..FONT_WIDTH {
//             let color = if (glyph_byte & (MASK_BASE >> cx)) != 0 { fgcolor } else { bgcolor };
//             if (glyph_byte & (MASK_BASE >> cx)) == 0 {
//                 continue; // "Transparency"
//             }
//             putpixel(vbe_data, color, x + (cx as isize), y + (cy as isize));
//         }
//     }
// }

pub fn print_string(
    str: &str,
    fgcolor: u64,
    bgcolor: u64,
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

pub fn fill_screen(vbe_data: &VbeModeInfo, color: u64) {
    let screen_x: isize = vbe_data.width as isize;
    let screen_y: isize = vbe_data.height as isize;
    let framebuffer = vbe_data.framebuffer as *mut u8;

    for y in 0..screen_y {
        for x in 0..screen_x {
            unsafe {
                let offset = y * (vbe_data.pitch as isize) + x * ((vbe_data.bpp as isize) / 8);
                *framebuffer.wrapping_offset(offset as isize) = color as u8;
            }
        }
    }
}

// This function is called on panic.
#[panic_handler]
fn panic(_info: &PanicInfo) -> ! {
    unsafe {
        asm!("mov dx, 0xE9");
        asm!("mov al, {}", in(reg_byte) ('!' as u8)); // 21h
        asm!("out dx, al");
        asm!("mov al, {}", in(reg_byte) ('E' as u8)); // 45h
        asm!("out dx, al");
        asm!("mov al, {}", in(reg_byte) ('E' as u8)); // 45h
        asm!("out dx, al");
        asm!("mov al, {}", in(reg_byte) ('!' as u8)); // 21h
        asm!("out dx, al");
        // fill_screen(&*VBE_DATA, 0xa);
        //drawchar('E', 0, 0, 0x04, 0x00, &*VBE_DATA);
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
    kmain();
    // END
    loop {
    }
}

#[no_mangle]
fn kmain() -> ! {
    unsafe {
        // let hello_world: &'static str = "Hello, world!";
        let vbe_data: &VbeModeInfo = &*VBE_DATA;
        // fill_screen(vbe_data, 0xA);
        // for _h in 0..8 {
        //     // Fill the screen (rainbow)
        //     for i in 0..0xff {
        //         fill_screen(vbe_data, i);
        //         // Beyond the 512K barrier we crash
        //     }
        // }
        // Output to emulated serial port
        // asm!("mov dx, 0xE9");
        // asm!("mov al, {}", in(reg_byte) ('A' as u8)); // 41h
        // asm!("out dx, al");
        // asm!("mov al, {}", in(reg_byte) ('B' as u8)); // 41h
        // asm!("out dx, al");
        // asm!("mov al, {}", in(reg_byte) ('C' as u8)); // 41h
        // asm!("out dx, al");
        // asm!("mov al, {}", in(reg_byte) ('D' as u8)); // 41h
        // asm!("out dx, al");
        // asm!("mov al, {}", in(reg_byte) ('E' as u8)); // 41h
        // asm!("out dx, al");
        // asm!("mov al, {}", in(reg_byte) (' ' as u8)); // 41h
        // asm!("out dx, al");
        // asm!("mov al, {}", in(reg_byte) ('F' as u8)); // 41h
        // asm!("out dx, al");
        // asm!("mov al, {}", in(reg_byte) ('G' as u8)); // 41h
        // asm!("out dx, al");
        // print_string("TEST", 0x2, 0x00, 30, 10, vbe_data);
        // putpixel(vbe_data, 0x2, 0, 0);
        // putpixel(vbe_data, 0x2, 1, 0);
        // putpixel(vbe_data, 0x2, 2, 0);
        // putpixel(vbe_data, 0x2, 3, 0);
        // putpixel(vbe_data, 0x2, 4, 0);
        x_string("HELO", 0x02, 0x00, 60, 30, &*VBE_DATA); // <--- This code is giving trouble @todo Fix.
        drawchar('A', 30, 30, 0x2, 0x00, vbe_data);
        drawchar('B', 38, 30, 0x2, 0x00, vbe_data);
        drawchar('C', 46, 30, 0x2, 0x00, vbe_data);
        drawchar('A', 60, 30, 0x2, 0x00, &*VBE_DATA);
        drawchar('B', 68, 30, 0x2, 0x00, vbe_data);
        drawchar('C', 76, 30, 0x2, 0x00, vbe_data);
    }
    loop {
    }
}
