#![no_std]
#![no_main]

use core::panic::PanicInfo;

/// This function is called on panic.
#[panic_handler]
fn panic(_info: &PanicInfo) -> ! {
    loop {
    }
}


// static HELLO: &[u8] = b"Hello World!";

#[no_mangle]
#[link_section = ".text.init"] 
pub unsafe extern "C" fn _start() -> ! {
    unsafe {
        let vga = 0xA0000 as *mut u32;
        *vga.offset((320/2)+(200/2)*320) = 0x0A; // Should display a pixel in the center of the screen
    };

    loop {
    }
}