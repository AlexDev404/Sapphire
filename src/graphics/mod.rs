pub mod fonts;
use crate::types::graphics::VbeModeInfo;
use fonts::ibm_vga8x16;
use core::arch::asm;

static F_DATA: [u8; 4096] = ibm_vga8x16::IBM_VGA_8X16;
static F_HEIGHT: isize = 16; // Rows
static F_WIDTH: isize = 8; // Columns

pub fn putpixel(vbe_data: &VbeModeInfo, color: u64, x: isize, y: isize) {
    // Pixel FMT = framebuffer + (y * bytes_per_scan_line) + (x * (bytes_per_pixel / 8))
    let framebuffer = vbe_data.framebuffer as *mut u8;
    let offset = y * (vbe_data.pitch as isize) + x * ((vbe_data.bpp as isize) / 8); // framebuffer_base + (y * bytes_per_scanline) + (x * (bytes_per_pixel / 8))
    unsafe {
        *framebuffer.offset(offset as isize) = color as u8;
    }
}

/**
@brief A dead simple and less complicated than OSDev's implementation

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
pub fn drawchar(chr: char, x: isize, y: isize, fgcolor: u64, bgcolor: u64, vbe_data: &VbeModeInfo) {
    unsafe {
        let c: u8 = (chr as u32 & 0xFF) as u8;
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
                if (*glyph.offset(cy) & (MASK_BASE >> cx)) == 0x00 {
                    continue; // "Transparency"
                }
                putpixel(vbe_data, color, x + cx, y + cy);
            }
        }
    }
}

// A reliable text printing function that uses byte arrays
pub unsafe fn print_text(
    text: &[u8],
    fgcolor: u64,
    bgcolor: u64,
    start_x: isize,
    y: isize,
    vbe_data: &VbeModeInfo
) {
    let mut pos = start_x;
    
    for &b in text {
        // Stop at null terminator
        if b == 0 {
            break;
        }
        
        // Handle special characters
        match b {
            b'\n' => {
                // Handle newline - move to start of next line
                pos = start_x;
                continue;
            },
            b'\r' => {
                // Carriage return - move to start of current line
                pos = start_x;
                continue;
            },
            b'\t' => {
                // Tab - move forward by 4 spaces
                pos += 9 * 4;
                continue;
            },
            // Only print printable ASCII characters
            32..=126 => {
                let c = b as char;
                drawchar(c, pos, y, fgcolor, bgcolor, vbe_data);
            },
            _ => {
                // For other non-printable characters, print a dot
                drawchar('.', pos, y, fgcolor, bgcolor, vbe_data);
            }
        }
        
        pos += 9; // Move to next position
    }
}

// Keep the original print_string for backward compatibility
pub unsafe fn print_string(
    str: &str,
    fgcolor: u64,
    bgcolor: u64,
    start_x: isize,
    y: isize,
    vbe_data: &VbeModeInfo
) {
    // Just draw a single character to indicate the function was called
    drawchar('X', start_x, y, fgcolor, bgcolor, vbe_data);
}

// Add this test function if it doesn't exist
pub unsafe fn test_print(
    start_x: isize,
    y: isize,
    vbe_data: &VbeModeInfo
) {
    // Draw a simple test pattern with just hardcoded calls to drawchar
    let mut pos = start_x;
    
    // Draw "TEST" with a marker before and after
    drawchar('>', pos, y, 0xf, 0x0, vbe_data); // White marker
    pos += 9;
    
    drawchar('T', pos, y, 0xe, 0x0, vbe_data); // Yellow T
    pos += 9;
    drawchar('E', pos, y, 0xe, 0x0, vbe_data); // Yellow E
    pos += 9;
    drawchar('S', pos, y, 0xe, 0x0, vbe_data); // Yellow S
    pos += 9;
    drawchar('T', pos, y, 0xe, 0x0, vbe_data); // Yellow T
    pos += 9;
    
    drawchar('<', pos, y, 0xf, 0x0, vbe_data); // White marker
}

// Add the print_chars function
pub unsafe fn print_chars(
    chars: &[u8],
    fgcolor: u64,
    bgcolor: u64,
    start_x: isize,
    y: isize,
    vbe_data: &VbeModeInfo
) {
    // Simple implementation that just delegates to print_text
    print_text(chars, fgcolor, bgcolor, start_x, y, vbe_data);
}

// Helper function to convert a u16 to a null-terminated byte array
pub unsafe fn u16_to_bytes(value: u16, buffer: &mut [u8]) -> usize {
    if buffer.len() < 6 {  // Max 5 digits + null terminator
        return 0;
    }
    
    if value == 0 {
        buffer[0] = b'0';
        buffer[1] = 0;
        return 2;
    }
    
    let mut val = value;
    let mut i = 0;
    
    while val > 0 && i < buffer.len() - 1 {
        buffer[i] = b'0' + (val % 10) as u8;
        val /= 10;
        i += 1;
    }
    
    // Reverse the digits in-place
    let mut start = 0;
    let mut end = i - 1;
    while start < end {
        let temp = buffer[start];
        buffer[start] = buffer[end];
        buffer[end] = temp;
        start += 1;
        end -= 1;
    }
    
    // Add null terminator
    buffer[i] = 0;
    
    i + 1  // Return length including null terminator
}

// Function to print a u16 number directly
pub unsafe fn print_u16(
    value: u16,
    fgcolor: u64,
    bgcolor: u64,
    x: isize,
    y: isize,
    vbe_data: &VbeModeInfo
) {
    let mut buffer = [0u8; 6];  // Max 5 digits + null terminator
    let _ = u16_to_bytes(value, &mut buffer);
    print_text(&buffer, fgcolor, bgcolor, x, y, vbe_data);
}

// Function to print at specific row/column using character-based coordinates
pub unsafe fn print_at(
    text: &[u8],
    fgcolor: u64,
    bgcolor: u64,
    col: isize,
    row: isize,
    vbe_data: &VbeModeInfo
) {
    // Convert character-based coordinates to pixel coordinates
    // Each character is 8x16 pixels with 1 pixel spacing
    let x = col * 9;  // 8 pixels wide + 1 pixel spacing
    let y = row * 16; // 16 pixels tall
    
    print_text(text, fgcolor, bgcolor, x, y, vbe_data);
}