#![no_std]
#![no_main]
#[macro_use]
extern crate lazy_static;

mod graphics;
mod types;
use core::panic::PanicInfo;
use core::arch::asm;
use tinyvec_string::ArrayString;
use types::graphics::VbeModeInfo;

pub fn putpixel(vbe_data: &VbeModeInfo, color: u8, x: isize, y: isize) {
    // Pixel FMT = framebuffer + (y * bytes_per_scan_line) + (x * bytes_per_pixel)
    let framebuffer = vbe_data.framebuffer as *mut u8;
    let offset = y * (vbe_data.pitch as isize) + x * (vbe_data.bpp as isize); // framebuffer_base + (y * bytes_per_scanline) + (x * bytes_per_pixel)
    unsafe {
        *framebuffer.offset(offset as isize) = color;
    }
}

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
    putpixel(vbe_data, 0x02, 10, 9);

    // END
    loop {
    }
}
