#![no_std]
#![no_main]
extern crate lazy_static;

mod graphics;
mod types;
use crate::types::graphics::VbeModeInfo;
use core::panic::PanicInfo;
use core::arch::asm;
use crate::graphics::putpixel;
use crate::graphics::print_string as x_string;
use crate::graphics::drawchar as x_drawchar;
// static mut VBE_DATA: *const VbeModeInfo = 0 as *const _;

use crate::graphics::fonts::ibm_vga8x16;

static FONT_DATA: [u8; 4096] = ibm_vga8x16::IBM_VGA_8X16;
static FONT_HEIGHT: isize = 16; // Rows
static FONT_WIDTH: isize = 8; // Columns

/*
 * ISSUES PRESENT
 * ==============
 *
 * The issue lies when importing functions. They are working properly, but once
 * imported, the code gets skipped.
 */

fn u16_to_str(value: u16) -> &'static str {
    static mut BUFFER: [u8; 5] = [0; 5];
    let mut buffer = itoa::Buffer::new();
    let s = buffer.format(value);
    unsafe {
        BUFFER[..s.len()].copy_from_slice(s.as_bytes());
        core::str::from_utf8_unchecked(&BUFFER[..s.len()])
    }
}

fn drawchar(chr: char, x: isize, y: isize, fgcolor: u64, bgcolor: u64, vbe_data: &VbeModeInfo) {
    unsafe {
        let c: u8 = chr as u8;
        let font: *const u8 = &FONT_DATA as *const u8;
        let glyph = font.offset((c as isize) * FONT_HEIGHT);

        const MASK_BASE: u8 = 0x80;
        for cy in 0..FONT_HEIGHT {
            // x_drawchar('H', 30 + cy + 16, 200, 0xa, 0x0, &vbe_data);
            for cx in 0..FONT_WIDTH {
                // x_drawchar('H', 30, 200 + cx + 16, 0xa, 0x0, &vbe_data);
                let color = if (*glyph.offset(cy) & (MASK_BASE >> cx)) != 0x00 {
                    fgcolor
                } else {
                    bgcolor
                };
                if (*glyph.offset(cy) & (MASK_BASE >> cx)) == 0x00 {
                    continue; // "Transparency"
                }
                putpixel(vbe_data, color, x + cx, y + cy);
            }
        }
    }
}

fn print_string(
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
    //         asm!("mov dx, 0xE9");
    // asm!("mov al, {}", in(reg_byte) ('A' as u8)); // 41h
    // asm!("out dx, al");
        drawchar('X', 30 + pos, 200, 0xa, 0x0, &vbe_data);
        drawchar(chr, 30 + pos, 200, 0xa, 0x0, &vbe_data);
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
pub unsafe extern "C" fn _rust(vbe_mode_info: &VbeModeInfo) -> ! {
    // Internal function
    // x_string(
    //     "HELLO WORLD THESE ARE SOME WORDS HELLO WORLD THESE ARE SOME WORDS HELLO",
    //     0x02,
    //     0x00,
    //     30,
    //     110,
    //     &vbe_mode_info
    // ); // <--- Code gets skipped
    kmain(&vbe_mode_info);
    // print_string(u16_to_str(vbe_mode_info.width), 0x2, 0x00, 60, 80, &vbe_data);
    // ---------- UNDEFINED BEHAVIOR: Run after ----------
    // asm!("mov dx, 0xE9");
    // asm!("mov al, {}", in(reg_byte) ('A' as u8)); // 41h
    // asm!("out dx, al");
    // ---------- UNDEFINED BEHAVIOR ----------
    // END
    loop {
    }
}

#[no_mangle]
unsafe fn kmain(vbe_mode_info: &VbeModeInfo) -> () {
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
    // putpixel(vbe_data, 0x2, 0, 0);
    // putpixel(vbe_data, 0x2, 1, 0);
    // putpixel(vbe_data, 0x2, 2, 0);
    // putpixel(vbe_data, 0x2, 3, 0);
    // putpixel(vbe_data, 0x2, 4, 0);
    let hello_world: &'static str = "Hello, world!";
    x_drawchar('A', 30, 50, 0x2, 0x00, &vbe_mode_info);
    x_drawchar('B', 38, 50, 0x2, 0x00, &vbe_mode_info);
    x_drawchar('C', 46, 50, 0x2, 0x00, &vbe_mode_info);
    x_drawchar('X', 30, 70, 0x2, 0x00, &vbe_mode_info);
    x_drawchar('Y', 38, 70, 0x2, 0x00, &vbe_mode_info);
    x_drawchar('Z', 46, 70, 0x2, 0x00, &vbe_mode_info);
    x_string(
        "HELLO WORLD THESE ARE SOME WORDS HELLO WORLD THESE ARE SOME WORDS HELLO",
        0x02,
        0x00,
        30,
        110,
        &vbe_mode_info
    ); // <--- Code gets skipped
    // print_string("Width", 0x2, 0x00, 30, 140, &vbe_mode_info);
    print_string(&hello_world, 0x2, 0x00, 30, 170, &vbe_mode_info);  // <--- Code gets skipped
}
