pub mod fonts;
use tinyvec_string::ArrayString;
use crate::types::graphics::VbeModeInfo;
use fonts::ibm_vga8x16;

static F_DATA: [u8; 4096] = ibm_vga8x16::IBM_VGA_8X16;
static F_HEIGHT: isize = 16; // Rows
static F_WIDTH: isize = 8; // Columns

pub fn putpixel(vbe_data: &VbeModeInfo, color: u8, x: isize, y: isize) {
    // Pixel FMT = framebuffer + (y * bytes_per_scan_line) + (x * (bytes_per_pixel / 8))
    let framebuffer = vbe_data.framebuffer as *mut u8;
    let offset = y * (vbe_data.pitch as isize) + x * ((vbe_data.bpp as isize) / 8); // framebuffer_base + (y * bytes_per_scanline) + (x * (bytes_per_pixel / 8))
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
pub fn drawchar(chr: char, x: isize, y: isize, fgcolor: u8, bgcolor: u8, vbe_data: &VbeModeInfo) {
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
                putpixel(vbe_data, color, x + cx, y + cy);
            }
        }
    }
}

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
