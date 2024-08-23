#![no_std]
#![no_main]
#[macro_use]
extern crate lazy_static;

mod graphics;
mod types;
mod ibm_vga8x16;
use core::panic::PanicInfo;
use core::arch::asm;
use tinyvec_string::ArrayString;
use types::graphics::VbeModeInfo;

static F_DATA: [u8; 4096] = ibm_vga8x16::IBM_VGA_8X16;
static F_HEIGHT: isize = 16; // Rows
static F_WIDTH: isize = 8; // Columns

pub fn putpixel(vbe_data: &VbeModeInfo, color: u8, x: isize, y: isize) {
    // Pixel FMT = framebuffer + (y * bytes_per_scan_line) + (x * bytes_per_pixel)
    let framebuffer = vbe_data.framebuffer as *mut u8;
    let offset = y * (vbe_data.pitch as isize) + x * (vbe_data.bpp as isize); // framebuffer_base + (y * bytes_per_scanline) + (x * bytes_per_pixel)
    unsafe {
        *framebuffer.offset(offset as isize) = color;
    }
}

/**
@brief A dead simple and less complicated than OSDev's implementation

How the font is stored:
            row, row, row, ...
character... x,   x,   x,   x,  x,  x,  x,  x

Each character is represented by a row and each column represents a row of the character
Eight columns in the row for the eight rows in the character

|- 8 columns for every dimension's row representing a character as hex with each bit representing a character
|----------------|
  x x x x x x x
x               x   --- 8 rows
x     x   x     x   |
x   x x x       x   |
x     x x       x   |
x              x    |
  x x x x x x x     ---


What it does:
1. Read the font from the array
2. Get the character's offset
3. Loop through the [character][row] and taking the element and converting from hex to binary
4. offset into the array???

*/
fn drawchar(chr: char, x: isize, y: isize, fgcolor: u8, bgcolor: u8, vbe_data: &VbeModeInfo) {
    unsafe {
        let c: u8 = chr as u8;
        let font: *const u8 = &F_DATA as *const u8;
        let glyph = font.offset((c as isize) * F_HEIGHT);

        const MASK_BASE: u8 = 0x80;

        for cy in 0..F_HEIGHT {
            for cx in 0..F_WIDTH {
                let color = if (*glyph.offset(cy) & (MASK_BASE >> cx)) != 0x00 {
                    fgcolor
                } else {
                    bgcolor
                };
                putpixel(vbe_data, color, x + cx, y + cy - 12);
            }
        }
    }
}
// fn drawchar(chr: char, x: isize, y: isize, fgcolor: u8, bgcolor: u8, vbe_data: &VbeModeInfo) {
//     unsafe {
//         let c: u8 = chr as u8;
//         let font = &F_DATA as *const u8;

//         // Calculate the offset of the glyph data for the given character
//         let glyph_size: isize = ((F_WIDTH as isize) * (F_HEIGHT as isize)) / (8 as isize); // Number of bytes per glyph
//         let glyph_offset: isize = (c as isize) * glyph_size; // Offset of the glyph data for this character
//         let glyph = *font.offset(glyph_offset as isize) as *const u8;

//         // Iterate over each pixel in the glyph data and draw it to the screen
//         for cy in 0..F_HEIGHT {
//             putpixel(vbe_data, 0x2, x, y);
//             for cx in 0..F_WIDTH {
//                 // let color = if (*glyph.offset(cx as isize) & (0x80 >> cx)) != 0x00 {
//                 //     fgcolor
//                 // } else {
//                 //     bgcolor
//                 // };
//                 // putpixel(vbe_data, color, x + cx, y + cy);
//                 putpixel(vbe_data, 0x2, x + cx, y + cy);

//             }
//         }

//     }
// }

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
    putpixel(vbe_data, 0x02, 9, 1);
    putpixel(vbe_data, 0x02, 10, 1);
    putpixel(vbe_data, 0x02, 10, 2);
    // putpixel(vbe_data, 0x02, 10, 3);
    // putpixel(vbe_data, 0x02, 10, 4);
    // putpixel(vbe_data, 0x02, 10, 5);
    // putpixel(vbe_data, 0x02, 10, 6);
    // putpixel(vbe_data, 0x02, 10, 7);
    // putpixel(vbe_data, 0x02, 10, 8);
    // putpixel(vbe_data, 0x02, 10, 9);
    // putpixel(vbe_data, 0x02, 10, 10);
    // putpixel(vbe_data, 0x02, 10, 11);
    // putpixel(vbe_data, 0x02, 10, 12);
    // putpixel(vbe_data, 0x02, 10, 13);
    // putpixel(vbe_data, 0x02, 10, 14);
    // putpixel(vbe_data, 0x02, 10, 15);
    drawchar('A', 30, 30, 0x2, 0x2, vbe_data);

    // END
    loop {
    }
}
