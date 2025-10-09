#![no_std]
#![no_main]
extern crate lazy_static;

mod graphics;
mod types;
use crate::types::graphics::VbeModeInfo;
use core::panic::PanicInfo;
use core::arch::asm;
use crate::graphics::putpixel;
use crate::graphics::print_string;
use crate::graphics::drawchar;
use crate::graphics::test_print;
use crate::graphics::print_chars;
use crate::graphics::print_text;
use crate::graphics::print_u16;
use crate::graphics::print_at;
use crate::graphics::fonts::ibm_vga8x16;

static FONT_DATA: [u8; 4096] = ibm_vga8x16::IBM_VGA_8X16;
static FONT_HEIGHT: isize = 16; // Rows
static FONT_WIDTH: isize = 8; // Columns

/*
 * ISSUES PRESENT
 * ==============
 *
 * Strings aren't being passed to the function `print_string` correctly
 *    --> Strings are being passed to the function as blanks but yet
 *        the function is able to know how many characters are in the string
 */

// fn u16_to_str(value: u16) -> &'static str {
//     static mut BUFFER: [u8; 5] = [0; 5];
//     let mut buffer = itoa::Buffer::new();
//     let s = buffer.format(value);
//     unsafe {
//         BUFFER[..s.len()].copy_from_slice(s.as_bytes());
//         core::str::from_utf8_unchecked(&BUFFER[..s.len()])
//     }
// }
/*
fn drawchar(chr: char, x: isize, y: isize, fgcolor: u64, bgcolor: u64, vbe_data: &VbeModeInfo) {
    unsafe {
        let c: u8 = (chr as u32 & 0xFF) as u8;
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
        unsafe {
            // Used to debug current character being drawn to the screen
            asm!("mov dx, 0xE9");
            asm!("mov al, {}", in(reg_byte) (chr as u8)); // 41h
            asm!("out dx, al");
        }
    }
}*/
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
fn kmain(vbe_mode_info: &VbeModeInfo) -> () {
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
    drawchar('A', 30, 50, 0x2, 0x00, &vbe_mode_info);
    drawchar('B', 38, 50, 0x2, 0x00, &vbe_mode_info);
    drawchar('C', 46, 50, 0x2, 0x00, &vbe_mode_info);
    drawchar('X', 30, 70, 0x2, 0x00, &vbe_mode_info);
    drawchar('Y', 38, 70, 0x2, 0x00, &vbe_mode_info);
    drawchar('Z', 46, 70, 0x2, 0x00, &vbe_mode_info);
    unsafe {
        // Comment out all the complex examples for now
        print_string(
            "HELLO WORLD THESE ARE SOME WORDS HELLO WORLD THESE ARE SOME WORDS HELLO",
            0x02,
            0x00,
            30,
            90,
            &vbe_mode_info
        );

        // print_string("Width", 0x2, 0x00, 30, 140, &vbe_mode_info);
        // print_string(&hello_world, 0x2, 0x00, 30, 170, &vbe_mode_info);

        // Keep only the simplest working code
        test_print(30, 200, &vbe_mode_info);

        // Use our basic print_text function with a simple byte array
        let hello_bytes = [
            b'H',
            b'e',
            b'l',
            b'l',
            b'o',
            b' ',
            b'W',
            b'o',
            b'r',
            b'l',
            b'd',
            b'!',
            0, // Null-terminated
        ];

        print_text(&hello_bytes, 0x4, 0x0, 30, 230, &vbe_mode_info);

        // Demonstrate special characters handling
        let special_chars = [
            b'T',
            b'a',
            b'b',
            b':',
            b'\t',
            b'A',
            b'f',
            b't',
            b'e',
            b'r',
            b' ',
            b't',
            b'a',
            b'b',
            0,
        ];
        print_text(&special_chars, 0x3, 0x0, 30, 260, &vbe_mode_info);

        // Display system info
        let system_info = [
            b'S',
            b'a',
            b'p',
            b'p',
            b'h',
            b'i',
            b'r',
            b'e',
            b' ',
            b'O',
            b'S',
            b' ',
            b'v',
            b'0',
            b'.',
            b'1',
            0,
        ];
        print_text(&system_info, 0xe, 0x0, 30, 290, &vbe_mode_info);

        // Show screen resolution (manually constructed)
        let resolution_prefix = [b'R', b'e', b's', b':', b' ', 0];
        print_text(&resolution_prefix, 0x2, 0x0, 30, 320, &vbe_mode_info);

        // Display width digit by digit
        let mut width_pos = 30 + 9 * 5; // Position after "Res: "
        let mut width = vbe_mode_info.width;

        // Convert width to digits and display
        let mut digits = [0u8; 5];
        let mut i = 0;

        if width == 0 {
            digits[i] = b'0';
            i += 1;
        } else {
            while width > 0 && i < 5 {
                digits[i] = b'0' + ((width % 10) as u8);
                width /= 10;
                i += 1;
            }
        }

        // Display digits in reverse (most significant first)
        for j in (0..i).rev() {
            let digit = [digits[j], 0];
            print_text(&digit, 0x2, 0x0, width_pos, 320, &vbe_mode_info);
            width_pos += 9;
        }

        // Add "x" between width and height
        let x_char = [b'x', 0];
        print_text(&x_char, 0x2, 0x0, width_pos, 320, &vbe_mode_info);
        width_pos += 9;

        // Display height digit by digit
        let mut height = vbe_mode_info.height;
        i = 0;

        if height == 0 {
            digits[i] = b'0';
            i += 1;
        } else {
            while height > 0 && i < 5 {
                digits[i] = b'0' + ((height % 10) as u8);
                height /= 10;
                i += 1;
            }
        }

        // Display digits in reverse (most significant first)
        for j in (0..i).rev() {
            let digit = [digits[j], 0];
            print_text(&digit, 0x2, 0x0, width_pos, 320, &vbe_mode_info);
            width_pos += 9;
        }

        // Now demonstrate our new utility functions

        // 1. Use print_u16 to display numbers directly
        let width_label = [b'W', b'i', b'd', b't', b'h', b':', b' ', 0];
        print_text(&width_label, 0xa, 0x0, 30, 350, &vbe_mode_info);
        print_u16(vbe_mode_info.width, 0xa, 0x0, 30 + 9 * 7, 350, &vbe_mode_info);

        let height_label = [b'H', b'e', b'i', b'g', b'h', b't', b':', b' ', 0];
        print_text(&height_label, 0xa, 0x0, 30, 370, &vbe_mode_info);
        print_u16(vbe_mode_info.height, 0xa, 0x0, 30 + 9 * 8, 370, &vbe_mode_info);

        // 2. Use print_at to position text using row/column coordinates
        let row_label = [
            b'R',
            b'o',
            b'w',
            b'/',
            b'C',
            b'o',
            b'l',
            b' ',
            b'P',
            b'o',
            b's',
            b'i',
            b't',
            b'i',
            b'o',
            b'n',
            b'i',
            b'n',
            b'g',
            b':',
            0,
        ];
        print_at(&row_label, 0xb, 0x0, 3, 25, &vbe_mode_info);

        // Display text at different positions using row/column coordinates
        let row_1 = [b'T', b'o', b'p', b' ', b'L', b'e', b'f', b't', 0];
        let row_2 = [b'T', b'o', b'p', b' ', b'R', b'i', b'g', b'h', b't', 0];
        let row_3 = [b'B', b'o', b't', b't', b'o', b'm', b' ', b'L', b'e', b'f', b't', 0];
        print_at(&row_1, 0xd, 0x0, 0, 0, &vbe_mode_info);
        print_at(&row_2, 0xd, 0x0, 70, 0, &vbe_mode_info);
        print_at(&row_3, 0xd, 0x0, 0, 28, &vbe_mode_info);
    }
}
