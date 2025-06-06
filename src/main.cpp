/*
 * Sapphire OS - Main entry point
 * C++ kernel implementation
 */

#include "graphics/graphics.h"
#include "graphics/print_text.h"
#include "devices/debug/debug.h"

void main(const VbeModeInfo* vbe_mode_info);
void panic(const VbeModeInfo* vbe_mode_info);

extern "C" void kmain(const VbeModeInfo* vbe_mode_info) {
    // This is our kernel entry point that will be called from the bootloader
    main(vbe_mode_info);
    
    // This function will be called on system panic
    panic(vbe_mode_info);
}

// Define true color RGB values (works with 16-bit, 24-bit, and 32-bit color modes)
#define RGB(r,g,b) ((r << 16) | (g << 8) | b)

// Color constants in RGB format
#define COLOR_BLACK      RGB(0, 0, 0)
#define COLOR_BLUE       RGB(0, 0, 170)
#define COLOR_DARK_BLUE  RGB(0, 0, 100)
#define COLOR_GREEN      RGB(0, 170, 0)
#define COLOR_CYAN       RGB(0, 170, 170)
#define COLOR_RED        RGB(170, 0, 0)
#define COLOR_MAGENTA    RGB(170, 0, 170)
#define COLOR_BROWN      RGB(170, 85, 0)
#define COLOR_LIGHT_GRAY RGB(170, 170, 170)
#define COLOR_DARK_GRAY  RGB(85, 85, 85)
#define COLOR_LIGHT_BLUE RGB(85, 85, 255)
#define COLOR_LIGHT_GREEN RGB(85, 255, 85)
#define COLOR_LIGHT_CYAN RGB(85, 255, 255)
#define COLOR_LIGHT_RED  RGB(255, 85, 85)
#define COLOR_LIGHT_MAGENTA RGB(255, 85, 255)
#define COLOR_YELLOW     RGB(255, 255, 85)
#define COLOR_WHITE      RGB(255, 255, 255)

// Kernel main function - entry point from bootloader
void main(const VbeModeInfo* vbe_mode_info) {
    // Fill screen with dark blue background
    fill_screen(vbe_mode_info, COLOR_DARK_BLUE);
    
    // Display welcome message with bright yellow on dark blue for good contrast
    const unsigned char welcome[] = "Sapphire OS v0.1 (C++ Edition)";
    print_text(welcome, COLOR_YELLOW, COLOR_DARK_BLUE, 30, 30, vbe_mode_info);
    
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
    const unsigned char color_text[] = "Supporting millions of colors!";
    print_text(color_text, COLOR_LIGHT_GREEN, COLOR_DARK_BLUE, 30, 100, vbe_mode_info);
    
    // Display test pattern with better visibility
    //test_print(30, 120, vbe_mode_info);
    
    // Display some sample text using row/column positioning
    const unsigned char row_1[] = "Top Left";
    const unsigned char row_2[] = "Top Right";
    const unsigned char row_3[] = "Bottom Left";
    print_at(row_1, COLOR_WHITE, COLOR_DARK_BLUE, 0, 0, vbe_mode_info);
    print_at(row_2, COLOR_WHITE, COLOR_DARK_BLUE, 70, 0, vbe_mode_info);
    print_at(row_3, COLOR_WHITE, COLOR_DARK_BLUE, 0, 28, vbe_mode_info);

    send_debug("Sapphire OS v0.1 (C++ Edition) initialized successfully.\n");
    send_debug("Hey, what's up!");
    
    // Draw a smooth, vibrant gradient showing off the color capabilities
    int gradient_height = 80;
    for (int x = 20; x < vbe_mode_info->width - 20; x++) {
        // Calculate position in the gradient as a value from 0 to 1
        float position = (float)(x - 20) / (float)(vbe_mode_info->width - 40);
        
        // Rainbow gradient effect with full RGB color spectrum
        unsigned long color;
        if (position < 0.16f) {
            // Red to yellow
            unsigned char r = 255;
            unsigned char g = (unsigned char)(255 * (position / 0.16f));
            unsigned char b = 0;
            color = RGB(r, g, b);
        } else if (position < 0.33f) {
            // Yellow to green
            unsigned char r = (unsigned char)(255 * (1.0f - ((position - 0.16f) / 0.17f)));
            unsigned char g = 255;
            unsigned char b = 0;
            color = RGB(r, g, b);
        } else if (position < 0.5f) {
            // Green to cyan
            unsigned char r = 0;
            unsigned char g = 255;
            unsigned char b = (unsigned char)(255 * ((position - 0.33f) / 0.17f));
            color = RGB(r, g, b);
        } else if (position < 0.66f) {
            // Cyan to blue
            unsigned char r = 0;
            unsigned char g = (unsigned char)(255 * (1.0f - ((position - 0.5f) / 0.16f)));
            unsigned char b = 255;
            color = RGB(r, g, b);
        } else if (position < 0.83f) {
            // Blue to magenta
            unsigned char r = (unsigned char)(255 * ((position - 0.66f) / 0.17f));
            unsigned char g = 0;
            unsigned char b = 255;
            color = RGB(r, g, b);
        } else {
            // Magenta to red
            unsigned char r = 255;
            unsigned char g = 0;
            unsigned char b = (unsigned char)(255 * (1.0f - ((position - 0.83f) / 0.17f)));
            color = RGB(r, g, b);
        }
        
        // Draw vertical line with the gradient color
        vline(x, vbe_mode_info->height - gradient_height, vbe_mode_info->height - 20, color, vbe_mode_info);
    }
    
    // Infinite loop to prevent a kernel panic
    while (1) {
        // Busy loop
    }
}

// Panic handler
void panic(const VbeModeInfo* vbe_mode_info) {
    // In a real system, we'd want to display an error message
    // For now, just an infinite loop
    send_debug("Kernel panic: An unrecoverable error occurred.\n");
    fill_screen(vbe_mode_info, COLOR_BLACK); // Fill screen with red to indicate panic
    print_text((const unsigned char*)"Kernel panic: An unrecoverable error occurred.", COLOR_RED, COLOR_BLACK, 30, 30, vbe_mode_info);
    // Draw some simple shapes to indicate panic state
    hline(30, 500, 50, COLOR_RED, vbe_mode_info); // Draw a horizontal line for visibility
    while (1) {
        // Busy loop
    }
}