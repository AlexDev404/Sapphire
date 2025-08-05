/*
 * Sapphire OS - Main entry point
 * C++ kernel implementation
 */

#include <graphics/print_text.hpp>
#include <devices/debug/debugcon.h>
#include <devices/init/init.hpp>
#include <graphics/stuff/stuff.h>
#include <platform/gdt.hpp>

void main(const VbeModeInfo *vbe_mode_info);
void panic();

extern "C" void kmain(const VbeModeInfo *vbe_mode_info)
{
    g_GDTInit(); // Initialize the Global Descriptor Table (GDT)
    send_debug("KMAIN: Entrypoint hit!\n");

    // DEBUG: Print critical memory addresses to debug console
    send_debug("=== MEMORY LAYOUT DEBUG ===\n");
    send_debug("Framebuffer address: 0x");
    // Convert framebuffer address to hex string for debugging
    unsigned long fb_addr = (unsigned long)vbe_mode_info->framebuffer;
    char hex_str[16];
    for (int i = 7; i >= 0; i--)
    {
        unsigned char nibble = (fb_addr >> (i * 4)) & 0xF;
        hex_str[7 - i] = (nibble < 10) ? ('0' + nibble) : ('A' + nibble - 10);
    }
    hex_str[8] = '\0';
    send_debug(hex_str);
    send_debug("\n");
    send_debug("========================\n");

    // Debug: Mark start of kmain
    putpixel(100, 50, 0xFF0000); // Red pixel = kmain started

    // Initialize devices, the system, and whatnot
    init();

    // Debug: Mark after init
    putpixel(102, 50, 0x00FF00); // Green pixel = init completed

    // This is our kernel entry point that will be called from the bootloader
    main(vbe_mode_info);

    // Debug: Mark after main (this probably won't be reached due to infinite loop in main)
    putpixel(104, 50, 0x0000FF); // Blue pixel = main completed

    // This function will be called on system panic
    panic();
}

// Kernel main function - entry point from bootloader
void main(const VbeModeInfo *vbe_mode_info)
{
    // // Initialize the console
    // send_debug("KERNEL MAIN: Initializing console...\n");
    // print_text("Sapphire OS Kernel", COLOR_WHITE, COLOR_DARK_BLUE, 30, 20);
    // send_debug("KERNEL MAIN: Console initialized.\n");
    // send_debug("KERNEL MAIN: Displaying welcome message...\n");

    // Do a little test where we use print_at to print throughout the whole 89 columns
    // And 24 rows of the screen
    // const int console_width = 85;  // Assuming 85 columns for the console
    // const int console_height = 25; // Assuming 25 rows for the console

    // // Clear the screen
    // for (int y = 0; y < console_height; y++)
    // {
    //     for (int x = 0; x < console_width; x++)
    //     {
    //         // Print 'a' for even positions, 'b' for odd positions
    //         if ((x + y) % 2 == 0) {
    //             print_at("a", COLOR_WHITE, COLOR_DARK_BLUE, x, y);
    //         } else {
    //             print_at("b", COLOR_WHITE, COLOR_DARK_BLUE, x, y);
    //         }
    //     }
    // }

    // Display welcome message
    print_text("What's up?", RGB(255, 199, 44), COLOR_DARK_BLUE, 30, 30);

    // Draw a box around the screen
    rect(10, 10, vbe_mode_info->width - 10, vbe_mode_info->height - 10, COLOR_WHITE);

    // Display resolution information
    print_text("Resolution: ", COLOR_LIGHT_CYAN, COLOR_DARK_BLUE, 30, 60);

    // Convert width to string and display
    unsigned char width_str[6];
    u16_to_string(vbe_mode_info->width, width_str, 6);
    print_text(reinterpret_cast<const char *>(width_str), COLOR_LIGHT_CYAN, COLOR_DARK_BLUE, 138, 60);

    print_text("x", COLOR_LIGHT_CYAN, COLOR_DARK_BLUE, 168, 60);

    // Convert height to string and display
    unsigned char height_str[6];
    u16_to_string(vbe_mode_info->height, height_str, 6);
    print_text(reinterpret_cast<const char *>(height_str), COLOR_LIGHT_CYAN, COLOR_DARK_BLUE, 178, 60);

    // Display color depth
    print_text("Color depth: ", COLOR_LIGHT_CYAN, COLOR_DARK_BLUE, 30, 80);

    // Convert bpp to string and display
    unsigned char bpp_str[6];
    u16_to_string(vbe_mode_info->bpp, bpp_str, 6);
    print_text(reinterpret_cast<const char *>(bpp_str), COLOR_LIGHT_CYAN, COLOR_DARK_BLUE, 146, 80);

    print_text("bits", COLOR_LIGHT_CYAN, COLOR_DARK_BLUE, 164, 80);

    // Add millions of colors text
    print_text("Color support: 16.8M colors (Mode 115h)", COLOR_LIGHT_GREEN, COLOR_DARK_BLUE, 30, 100);

    // Display some sample text using row/column positioning
    print_at("Top Left", COLOR_WHITE, COLOR_DARK_BLUE, 0, 0);
    print_at("Top Right", COLOR_WHITE, COLOR_DARK_BLUE, 70, 0);
    print_at("Bottom Left", COLOR_WHITE, COLOR_DARK_BLUE, 0, 28);

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
        vline(x, vbe_mode_info->height - gradient_height, vbe_mode_info->height - 20, color);
    }

    // Draw a circle in the center
    circle(vbe_mode_info->width / 2, vbe_mode_info->height / 2, 50, COLOR_WHITE);

    print_text("Hey, this text is coming from the vbe_mode_info", COLOR_LIGHT_GRAY, COLOR_BLACK, 30, 132);
    print_text("Hey, this text is coming from the vbe_block()", COLOR_LIGHT_MAGENTA, COLOR_BLACK, 30, 148, vbe_block());
    print_text("Hey, this text is coming from the vbe_mode_block", COLOR_LIGHT_MAGENTA, COLOR_BLACK, 30, 180, &vbe_mode_block);

    // Infinite loop to prevent a kernel panic
    while (1)
    {
        // Simple idle loop
    }
}

// Panic handler
void panic()
{
    send_debug("Kernel panic: An unrecoverable error occurred.\n");
    fill_screen(COLOR_BLACK);
    print_text("Kernel panic: An unrecoverable error occurred.", COLOR_RED, COLOR_BLACK, 30, 30);
    hline(30, 500, 50, COLOR_RED);
    while (1)
    {
        // Busy loop
    }
}
