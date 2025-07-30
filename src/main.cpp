/*
 * Sapphire OS - Main entry point
 * C++ kernel implementation
 */

#include "graphics/graphics.h"
#include "graphics/print_text.h"
#include "devices/debug/debugcon.h"
#include "devices/init/init.hpp"
#include "graphics/stuff/stuff.h"

void main(const VbeModeInfo *vbe_mode_info);
void panic(const VbeModeInfo *vbe_mode_info);

extern "C" void kmain(const VbeModeInfo *vbe_mode_info)
{
    send_debug("KMAIN: Entrypoint hit!\n");
    
    // DEBUG: Print critical memory addresses to debug console
    send_debug("=== MEMORY LAYOUT DEBUG ===\n");
    send_debug("Framebuffer address: 0x");
    // Convert framebuffer address to hex string for debugging
    unsigned long fb_addr = (unsigned long)vbe_mode_info->framebuffer;
    char hex_str[16];
    for (int i = 7; i >= 0; i--) {
        unsigned char nibble = (fb_addr >> (i * 4)) & 0xF;
        hex_str[7-i] = (nibble < 10) ? ('0' + nibble) : ('A' + nibble - 10);
    }
    hex_str[8] = '\0';
    send_debug(hex_str);
    send_debug("\n");
    send_debug("========================\n");
    
    // Debug: Mark start of kmain
    putpixel(100, 50, 0xFF0000, vbe_mode_info); // Red pixel = kmain started
    
    // Initialize devices, the system, and whatnot
    init(vbe_mode_info);
    
    // Debug: Mark after init
    putpixel(102, 50, 0x00FF00, vbe_mode_info); // Green pixel = init completed
    
    // This is our kernel entry point that will be called from the bootloader
    main(vbe_mode_info);

    // Debug: Mark after main (this probably won't be reached due to infinite loop in main)
    putpixel(104, 50, 0x0000FF, vbe_mode_info); // Blue pixel = main completed

    // This function will be called on system panic
    panic(vbe_mode_info);
}

// Kernel main function - entry point from bootloader
void main(const VbeModeInfo *vbe_mode_info)
{
    // Debug: Mark start of main function
    putpixel(150, 50, 0xFFFF00, vbe_mode_info); // Yellow pixel = main() started
    
    // Fill screen with dark blue background
    //fill_screen(vbe_mode_info, COLOR_DARK_BLUE);

    // Debug: Mark after fill_screen
    putpixel(152, 50, 0xFF00FF, vbe_mode_info); // Magenta pixel = after fill_screen

    // Display welcome message with bright yellow on dark blue for good contrast
    const unsigned char welcome[] = "What's up?";
    print_text(welcome, RGB(255, 199, 44), COLOR_DARK_BLUE, 30, 30, vbe_mode_info);

    // Draw a box around the screen - bright white for visibility
    rect(10, 10, vbe_mode_info->width - 10, vbe_mode_info->height - 10, COLOR_WHITE, vbe_mode_info);

    // Display resolution information - light cyan for better visibility
    const unsigned char res_text[] = "Resolution: ";
    print_text(res_text, COLOR_LIGHT_CYAN, COLOR_DARK_BLUE, 30, 60, vbe_mode_info);

    // Convert width to string and display
    unsigned char width_str[6];
    u16_to_string(vbe_mode_info->width, width_str, 6);
    print_text(width_str, COLOR_LIGHT_CYAN, COLOR_DARK_BLUE, 138, 60, vbe_mode_info);

    // Add 'x' between width and height
    const unsigned char x_char[] = "x";
    print_text(x_char, COLOR_LIGHT_CYAN, COLOR_DARK_BLUE, 168, 60, vbe_mode_info);

    // Convert height to string and display
    unsigned char height_str[6];
    u16_to_string(vbe_mode_info->height, height_str, 6);
    print_text(height_str, COLOR_LIGHT_CYAN, COLOR_DARK_BLUE, 178, 60, vbe_mode_info);

    // Display color depth
    const unsigned char bpp_text[] = "Color depth: ";
    print_text(bpp_text, COLOR_LIGHT_CYAN, COLOR_DARK_BLUE, 30, 80, vbe_mode_info);

    // Convert bpp to string and display
    unsigned char bpp_str[6];
    u16_to_string(vbe_mode_info->bpp, bpp_str, 6);
    print_text(bpp_str, COLOR_LIGHT_CYAN, COLOR_DARK_BLUE, 146, 80, vbe_mode_info);

    // Add "bits" after the number
    const unsigned char bits_text[] = "bits";
    print_text(bits_text, COLOR_LIGHT_CYAN, COLOR_DARK_BLUE, 164, 80, vbe_mode_info);

    // Add millions of colors text
    const unsigned char color_text[] = "Color support: 16.8M colors (Mode 115h)";
    print_text(color_text, COLOR_LIGHT_GREEN, COLOR_DARK_BLUE, 30, 100, vbe_mode_info);

    // Display test pattern with better visibility
    // test_print(30, 120, vbe_mode_info);

    // Display some sample text using row/column positioning
    const unsigned char row_1[] = "Top Left";
    const unsigned char row_2[] = "Top Right";
    const unsigned char row_3[] = "Bottom Left";
    print_at(row_1, COLOR_WHITE, COLOR_DARK_BLUE, 0, 0, vbe_mode_info);
    print_at(row_2, COLOR_WHITE, COLOR_DARK_BLUE, 70, 0, vbe_mode_info);
    print_at(row_3, COLOR_WHITE, COLOR_DARK_BLUE, 0, 28, vbe_mode_info);

    send_debug("\n\nOS initialized successfully.\n");
    send_debug("Hey, what's up!\n\n");

    // Test printing numbers
    for (int i = 0; i <= 10; i++)
    {
        char c = dbgp_int(i);
        send_debug(&c);
        send_debug(" ");
    }

    // Draw a smooth, vibrant gradient showing off the color capabilities
    int gradient_height = 80;
    for (int x = 20; x < vbe_mode_info->width - 20; x++)
    {
        // Calculate position in the gradient as a value from 0 to 1
        float position = (float)(x - 20) / (float)(vbe_mode_info->width - 40);

        // Rainbow gradient effect with full RGB color spectrum
        unsigned long color;
        if (position < 0.16f)
        {
            // Red to yellow
            unsigned char r = 255;
            unsigned char g = (unsigned char)(255 * (position / 0.16f));
            unsigned char b = 0;
            color = RGB(r, g, b);
        }
        else if (position < 0.33f)
        {
            // Yellow to green
            unsigned char r = (unsigned char)(255 * (1.0f - ((position - 0.16f) / 0.17f)));
            unsigned char g = 255;
            unsigned char b = 0;
            color = RGB(r, g, b);
        }
        else if (position < 0.5f)
        {
            // Green to cyan
            unsigned char r = 0;
            unsigned char g = 255;
            unsigned char b = (unsigned char)(255 * ((position - 0.33f) / 0.17f));
            color = RGB(r, g, b);
        }
        else if (position < 0.66f)
        {
            // Cyan to blue
            unsigned char r = 0;
            unsigned char g = (unsigned char)(255 * (1.0f - ((position - 0.5f) / 0.16f)));
            unsigned char b = 255;
            color = RGB(r, g, b);
        }
        else if (position < 0.83f)
        {
            // Blue to magenta
            unsigned char r = (unsigned char)(255 * ((position - 0.66f) / 0.17f));
            unsigned char g = 0;
            unsigned char b = 255;
            color = RGB(r, g, b);
        }
        else
        {
            // Magenta to red
            unsigned char r = 255;
            unsigned char g = 0;
            unsigned char b = (unsigned char)(255 * (1.0f - ((position - 0.83f) / 0.17f)));
            color = RGB(r, g, b);
        }

        // Draw vertical line with the gradient color
        vline(x, vbe_mode_info->height - gradient_height, vbe_mode_info->height - 20, color, vbe_mode_info);
    }

    // Stuff
    circle(vbe_mode_info->width / 2, vbe_mode_info->height / 2, 50, COLOR_WHITE, vbe_mode_info); // Draw a circle in the center

    // Infinite loop to prevent a kernel panic
    while (1)
    {
        // Simple idle loop - removed stack monitoring that was causing issues
    }
}

// Panic handler
void panic(const VbeModeInfo *vbe_mode_info)
{
    // In a real system, we'd want to display an error message
    // For now, just an infinite loop
    send_debug("Kernel panic: An unrecoverable error occurred.\n");
    fill_screen(vbe_mode_info, COLOR_BLACK); // Fill screen with red to indicate panic
    print_text((const unsigned char *)"Kernel panic: An unrecoverable error occurred.", COLOR_RED, COLOR_BLACK, 30, 30, vbe_mode_info);
    // Draw some simple shapes to indicate panic state
    hline(30, 500, 50, COLOR_RED, vbe_mode_info); // Draw a horizontal line for visibility
    while (1)
    {
        // Busy loop
    }
}