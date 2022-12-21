#![no_std]
#![no_main]

mod graphics;
mod ibm_vga8x16;
use core::panic::PanicInfo;
use graphics::putpixel;

static vga_addr: u32 = 0xa0000;

// This function is called on panic.
#[panic_handler]
fn panic(_info: &PanicInfo) -> ! {
    loop {
    }
}

fn drawchar(chr: char, x: isize, y: isize, fgcolor: u8, bgcolor: u8) {
    unsafe {
        let c: u8 = chr as u8;
        let vga = vga_addr as *mut u8;
        let font: *const u8 = &ibm_vga8x16::IBM_VGA_8x16 as *const u8;
        let mask: [i32; 8] = [1, 2, 4, 8, 16, 32, 64, 128];
        let glyph = font.offset((c as isize)*16);

        // Initialize cx and cy to zero
        let mut cx: usize = 0;
        let mut cy: isize = 0;

        while cy < 16 {
            while cx < 8 {
                putpixel(
                    vga,
                    if (*glyph.offset(cy) & (mask[cx] as u8)) != 0x00 {
                        fgcolor
                    } else {
                        bgcolor
                    },
                    x + (cx as isize),
                    y + cy - 12
                );
                cx += 1;
            }
            // Reset cx for the next row
            cx = 0;
            cy += 1;
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