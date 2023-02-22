#![allow(dead_code)]
use lazy_static;
use core::arch::asm;

lazy_static! {
    static ref PITCH: isize = {
        let pitch: isize;
        unsafe {
            asm!("mov {}, eax", out(reg) pitch);
        }
        pitch
    };
}

lazy_static! {
    static ref BPP: isize = {
        let bpp: isize;
        unsafe {
            asm!("mov {}, ecx", out(reg) bpp);
        }
        bpp
    };
}

pub fn fill_screen(vga: *mut u8, screen_x: isize, screen_y: isize, color: u8) {
    for x in 0..screen_x {
        for y in 0..screen_y {
            unsafe {
                *vga.offset(x + y * 320) = color;
            }
        }
    }
}

pub fn putpixel(vga: *mut u8, color: u8, x: isize, y: isize) {
    unsafe {
        let offset = y * *PITCH + x * *BPP;
        *vga.offset(offset) = color;
    }
    // pixel = framebuffer + y * bytes_per_scan_line + x * bytes_per_pixel
}