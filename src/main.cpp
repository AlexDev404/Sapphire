/*
 * Sapphire OS - Main entry point
 * C++ kernel implementation
 */

#include "graphics/graphics.h"
#include "graphics/print_text.h"

extern "C" {
    // This is our kernel entry point that will be called from the bootloader
    void kmain(const VbeModeInfo* vbe_mode_info);
    
    // This function will be called on system panic
    void panic();
}

// Kernel main function - entry point from bootloader
extern "C" void kmain(const VbeModeInfo* vbe_mode_info) {
    // Fill screen with dark blue background
    //fill_screen(vbe_mode_info, 0x1);
    
    // Display welcome message
    const unsigned char welcome[] = "Sapphire OS v0.1 (C++ Edition)";
    print_text(welcome, 0xe, 0x0, 30, 30, vbe_mode_info);
    
    // Draw a box around the screen
    rect(10, 10, vbe_mode_info->width - 10, vbe_mode_info->height - 10, 0xa, vbe_mode_info);
    
    // Display resolution information
    const unsigned char res_text[] = "Resolution: ";
    print_text(res_text, 0x7, 0x0, 30, 60, vbe_mode_info);
    
    // Convert width to string and display
    unsigned char width_str[6];
    u16_to_string(vbe_mode_info->width, width_str, 6);
    print_text(width_str, 0x7, 0x0, 120, 60, vbe_mode_info);
    
    // Add 'x' between width and height
    const unsigned char x_char[] = "x";
    print_text(x_char, 0x7, 0x0, 160, 60, vbe_mode_info);
    
    // Convert height to string and display
    unsigned char height_str[6];
    u16_to_string(vbe_mode_info->height, height_str, 6);
    print_text(height_str, 0x7, 0x0, 170, 60, vbe_mode_info);
    
    // Display color depth
    const unsigned char bpp_text[] = "Color depth: ";
    print_text(bpp_text, 0x7, 0x0, 30, 80, vbe_mode_info);
    
    // Convert bpp to string and display
    unsigned char bpp_str[3];
    u16_to_string(vbe_mode_info->bpp, bpp_str, 3);
    print_text(bpp_str, 0x7, 0x0, 120, 80, vbe_mode_info);
    
    // Add "bits" after the number
    const unsigned char bits_text[] = "bits";
    print_text(bits_text, 0x7, 0x0, 140, 80, vbe_mode_info);
    
    // Display test pattern
    //test_print(30, 120, vbe_mode_info);
    
    // Display some sample text using row/column positioning
    const unsigned char row_1[] = "Top Left";
    const unsigned char row_2[] = "Top Right";
    const unsigned char row_3[] = "Bottom Left";
    print_at(row_1, 0xd, 0x0, 0, 0, vbe_mode_info);
    print_at(row_2, 0xd, 0x0, 70, 0, vbe_mode_info);
    print_at(row_3, 0xd, 0x0, 0, 28, vbe_mode_info);
    
    // Draw a simple color gradient at the bottom
    for (int x = 20; x < vbe_mode_info->width - 20; x++) {
        unsigned long color = ((x * 255) / vbe_mode_info->width) << 16 | 
                            (((vbe_mode_info->width - x) * 255) / vbe_mode_info->width) << 8;
        vline(x, vbe_mode_info->height - 50, vbe_mode_info->height - 20, color, vbe_mode_info);
    }
    
    // Infinite loop to keep the system running
    while (1) {
        // Idle loop
    }
}

// Panic handler
extern "C" void panic() {
    // In a real system, we'd want to display an error message
    // For now, just an infinite loop
    while (1) {
        // Idle loop
    }
}