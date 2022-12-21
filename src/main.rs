#![no_std]
#![no_main]

mod graphics;
mod chunkymonkey_z80;
use core::panic::PanicInfo;
use graphics::putpixel;

static VGA_ADDR: u32 = 0xa0000;
static F_DATA: [u8; 768] = chunkymonkey_z80::CHUNKYMONKEY_Z80;
static F_HEIGHT: isize = 8;
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
        let glyph = font.offset((c as isize)*F_HEIGHT);


        const MASK_BASE: u8 = 0x80;

        for cy in 0..F_HEIGHT {
            for cx in 0..F_WIDTH {
                let color = if *glyph.offset(cy) & MASK_BASE >> cx != 0x00 { fgcolor } else { bgcolor };
                putpixel(vga, color, x + cx, y + cy - 12);
            }
        }
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
    drawchar('H', 30, 30, 0x0a, 0x00);
    drawchar('E', 60, 60, 0x0a, 0x00);
    drawchar('A', 90, 90, 0x0a, 0x00);
    // END

    loop {
    }
}