#![no_std]
#![no_main]
#[macro_use]
extern crate lazy_static;

mod graphics;
mod ibm_vga8x16;
use core::panic::PanicInfo;
use core::arch::asm;
use graphics::{ putpixel, fill_screen };
use tinyvec_string::ArrayString;

// static VGA_ADDR: u32 = 0xa0000;
lazy_static! {
    static ref VGA_ADDR: u32 = {
        let fb: *const u32;
        unsafe {
            asm!("mov {}, ebx", out(reg) fb);
        }
        fb as u32
    };
}

#[no_mangle]
#[allow(dead_code)]
pub unsafe extern "C" fn _testvbe() {
    // @audit TODO: Assign the framebuffer variable the value of EBX or find a way to store and access the framebuffer address from code
    // Start plot pixel
    let fb_addr: u32 = *VGA_ADDR;
    let fb: *const u32 = fb_addr as *const u32;
    asm!("mov ax, 0x0F");
    asm!("add {}, 180050", in(reg) fb);
    asm!("mov [{}], ax", in(reg) fb);
    // _rust();
}

static F_DATA: [u8; 4096] = ibm_vga8x16::IBM_VGA_8X16;
static F_HEIGHT: isize = 16; // Rows
static F_WIDTH: isize = 8; // Columns

// This function is called on panic.
#[panic_handler]
fn panic(_info: &PanicInfo) -> ! {
    loop {
    }
}

/**
@brief Very simple and less complicated than OSDev's implementation

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
fn drawchar(chr: char, x: isize, y: isize, fgcolor: u8, bgcolor: u8) {
    unsafe {
        let c: u8 = chr as u8;
        let fb_addr: u32 = *VGA_ADDR;
        let vga = fb_addr as *mut u8;
        let font = &F_DATA as *const u8;

        // Calculate the offset of the glyph data for the given character
        let glyph_size: isize = ((F_WIDTH as isize) * (F_HEIGHT as isize)) / (8 as isize); // Number of bytes per glyph
        let glyph_offset: isize = (c as isize) * glyph_size; // Offset of the glyph data for this character
        let glyph = *font.offset(glyph_offset as isize) as *const u8;
        
        // Iterate over each pixel in the glyph data and draw it to the screen
        for cy in 0..F_HEIGHT {
            for cx in 0..F_WIDTH {
                let color = if (*glyph.offset(cx as isize) & (0x80 >> cx)) != 0x00 {
                    fgcolor
                } else {
                    bgcolor
                };
                putpixel(vga, color, x + cx, y + cy - 12);
            }
        }
    }
}

fn print_string(str: ArrayString<[u8; 13]>, fgcolor: u8, bgcolor: u8, start_x: isize, y: isize) {
    // Stack is max 13?? Why?
    let mut pos: isize = start_x;
    for chr in str.chars() {
        drawchar(chr, pos, y, fgcolor, bgcolor);
        pos += 9; // Each character is 8 bytes wide and we need at least 1 byte of separation
    }
}

// static HELLO: &[u8] = b"Hello World!";

#[no_mangle]
// #[link_section = ".text.init"]
pub unsafe extern "C" fn _rust() -> ! {
    // Works
    /****************************************** */
    let fb_addr: u32 = *VGA_ADDR;
    let vga = fb_addr as *mut u8;
    // *vga.offset(6) = 0xF;
    /******************************* */
    // Pixel FMT: x+y*screen_x
    // Setting the unit of the memory to units
    // let vga = vga_addr as *mut u8;
    // putpixel(vga, 0x0a, 0, 0);
    // putpixel(vga, 0x0a, 10, 10);
    // putpixel(vga, 0x0a, 10, 10);
    fill_screen(vga, 640, 480, 0x0f); // Fill the screen with white
    drawchar('H', 30, 30, 0x0a, 0x00);
    drawchar('E', 39, 30, 0x0a, 0x00);
    drawchar('L', 48, 30, 0x0a, 0x00);
    drawchar('L', 57, 30, 0x0a, 0x00);
    drawchar('O', 66, 30, 0x0a, 0x00);
    drawchar('!', 75, 30, 0x0a, 0x00);
    // putpixel(vga, 0x0F, 0, 0);

    // print_string(ArrayString::<[u8; 13]>::from("HELLO!"), 0x0f, 0x00, 1, 17);
    // END

    loop {
    }
}