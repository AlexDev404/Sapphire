#![allow(dead_code)]
use lazy_static;
use core::arch::asm;

lazy_static! {
    static ref PITCH: u32 = {
        let pitch: u32;
        unsafe {
            asm!("mov {}, esi", out(reg) pitch);
        }
        pitch
    };
}

lazy_static! {
    static ref BPP: u32 = {
        let bpp: u32;
        unsafe {
            asm!("mov {}, edi", out(reg) bpp);
        }
        bpp
    };
}

pub fn fill_screen(vga: *mut u8, screen_x: isize, screen_y: isize, color: u8) {
    for x in 0..screen_x {
        for y in 0..screen_y {
            unsafe {
                // let offset = y * *PITCH + x * *BPP;
                // *vga.offset(offset) = color;
            }
        }
    }
}

pub fn putpixel(vga: *mut u8, color: u8, x: isize, y: isize) {
    unsafe {
        // let p: u8 = *PITCH;
        // let pitch = p as isize;
        // let b: usize = *BPP;
        // let bpp = b as isize;
        let offset = y as u32 * *PITCH + x as u32 * *BPP;
        *vga.offset(offset as isize) = color;
    }
    // pixel = framebuffer + y * bytes_per_scan_line + x * bytes_per_pixel
}
