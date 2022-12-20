
#![allow(dead_code)]
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
        *vga.offset(x + y * 320) = color;
    }
}
