#![no_std]
#![no_main]

mod graphics;
mod ibm_vga8x16;
use core::panic::PanicInfo;
use graphics::putpixel;
use tinyvec_string::ArrayString;

static VGA_ADDR: u32 = 0xa0000;
static F_DATA: [u8; 4096] = ibm_vga8x16::IBM_VGA_8X16;
static F_HEIGHT: isize = 16;
static F_WIDTH: isize = 8;

// This function is called on panic.
#[panic_handler]
fn panic(_info: &PanicInfo) -> ! {
    loop {
    }
}

fn drawchar(chr: char, x: isize, y: isize, fgcolor: u8, bgcolor: u8) {
    unsafe {
        let c: u8 = chr as u8;
        let vga = VGA_ADDR as *mut u8;
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
                putpixel(vga, color, x + cx, y + cy - 12);
            }
        }
    }
}

fn print_string(str: ArrayString<[u8; 13]>, fgcolor: u8, bgcolor: u8, start_x: isize, y: isize) { // Stack is max 13?? Why?
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
    // Pixel FMT: x+y*screen_x
    // Setting the unit of the memory to units
    // let vga = vga_addr as *mut u8;
    // putpixel(vga, 0x0a, 0, 0);
    // drawchar('H', 30, 30, 0x0a, 0x00);
    // drawchar('E', 39, 30, 0x0a, 0x00);
    // drawchar('L', 48, 30, 0x0a, 0x00);
    // drawchar('L', 57, 30, 0x0a, 0x00);
    // drawchar('O', 66, 30, 0x0a, 0x00);
    // drawchar('!', 75, 30, 0x0a, 0x00);
    print_string(ArrayString::<[u8; 13]>::from("HELLO!"), 0x0a, 0x00, 1, 17);
    // END

    loop {
    }
}