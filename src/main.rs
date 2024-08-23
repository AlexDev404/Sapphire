#![no_std]
#![no_main]
extern crate lazy_static;

mod graphics;
mod types;
use crate::types::graphics::VbeModeInfo;
use core::panic::PanicInfo;
use tinyvec_string::ArrayString;
use crate::graphics::print_string;
use crate::graphics::drawchar;

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
    // drawchar('A', 30, 30, 0x0a, 0x00, vbe_data);
    // drawchar('B', 38, 30, 0x0a, 0x00, vbe_data);
    // drawchar('C', 46, 30, 0x0a, 0x00, vbe_data);
    print_string(ArrayString::<[u8; 13]>::from("HELLO!"), 0x0a, 0x00, 1, 17, vbe_data);

    // END
    loop {
    }
}
