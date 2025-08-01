/*
 * Basic graphics primitives for the Sapphire OS
 */

#include "graphics.h"

#ifdef __cplusplus
extern "C" {
#endif

// Draw a single pixel to the framebuffer
void putpixel(int x, int y, unsigned long color) {
    const VbeModeInfo* vbe_data = vbe_block();
    
    // Handle invalid coordinates
    if (x < 0 || y < 0 || x >= vbe_data->width || y >= vbe_data->height) {
        return;
    }
    
    // Calculate base address in the framebuffer
    unsigned char* framebuffer = (unsigned char*)vbe_data->framebuffer;
    
    // Set bytes per pixel based on color depth
    int bytes_per_pixel = vbe_data->bpp / 8;
    
    // Calculate position in framebuffer
    int fb_offset = (y * vbe_data->pitch) + (x * bytes_per_pixel);
    
    // Draw the pixel with appropriate color format based on bit depth
    if (vbe_data->bpp == 32) {
        // 32 bits per pixel (RGBA)
        framebuffer[fb_offset + 0] = (color >> 0) & 0xFF;  // Blue
        framebuffer[fb_offset + 1] = (color >> 8) & 0xFF;  // Green
        framebuffer[fb_offset + 2] = (color >> 16) & 0xFF; // Red
        framebuffer[fb_offset + 3] = (color >> 24) & 0xFF; // Alpha
    }
    else if (vbe_data->bpp == 24) {
        // 24 bits per pixel (RGB)
        framebuffer[fb_offset + 0] = (color >> 0) & 0xFF;  // Blue
        framebuffer[fb_offset + 1] = (color >> 8) & 0xFF;  // Green
        framebuffer[fb_offset + 2] = (color >> 16) & 0xFF; // Red
    }
    else if (vbe_data->bpp == 16) {
        // 16 bits per pixel (typically 5-6-5 RGB)
        unsigned short pixel_color = (
            (((color >> 16) & 0xFF) >> 3) << 11 | // Red (5 bits)
            (((color >> 8) & 0xFF) >> 2) << 5 |   // Green (6 bits)
            (((color >> 0) & 0xFF) >> 3)          // Blue (5 bits)
        );
        
        // Write the 16-bit value
        *((unsigned short*)(framebuffer + fb_offset)) = pixel_color;
    }
    else if (vbe_data->bpp == 8) {
        // 8 bits per pixel (palette-based)
        // This would require a proper color palette setup
        // Using a simple grayscale approximation here
        // unsigned char gray = (
        //     (((color >> 16) & 0xFF) * 30 + 
        //      ((color >> 8) & 0xFF) * 59 + 
        //      ((color >> 0) & 0xFF) * 11) / 100
        // );
        framebuffer[fb_offset] = color;
    }
}

// Draw a horizontal line
void hline(int x1, int x2, int y, unsigned long color) {
    const VbeModeInfo* vbe_data = vbe_block();

    if (x1 > x2) {
        // Swap if needed to ensure x1 <= x2
        int temp = x1;
        x1 = x2;
        x2 = temp;
    }
    
    line(x1, y, x2, y, color);
}

// Draw a vertical line
void vline(int x, int y1, int y2, unsigned long color) {
    const VbeModeInfo* vbe_data = vbe_block();

    if (y1 > y2) {
        // Swap if needed to ensure y1 <= y2
        int temp = y1;
        y1 = y2;
        y2 = temp;
    }
    
    for (int y = y1; y <= y2; y++) {
        putpixel(x, y, color, vbe_data);
    }
}

// Draw a generic line
void line(int x1, int y1, int x2, int y2, unsigned long color) {
    const VbeModeInfo* vbe_data = vbe_block();

    const float m = (y2 - y1) / (x2 - x1);

    for (int x = x1; x <= x2; x++) {
        const float y = (m * x) + (y1 - m);
        putpixel(x, y, color, vbe_data);
    }
}

// Draw a rectangle outline
void rect(int x1, int y1, int x2, int y2, unsigned long color) {
    const VbeModeInfo* vbe_data = vbe_block();
    
    hline(x1, x2, y1, color); // Top
    vline(x1, y1, y2, color); // Left
    hline(x1, x2, y2, color); // Bottom
    vline(x2, y1, y2, color); // Right
}

// Draw a filled rectangle
void fillrect(int x1, int y1, int x2, int y2, unsigned long color) {
    const VbeModeInfo* vbe_data = vbe_block();

    if (x1 > x2) {
        // Swap if needed to ensure x1 <= x2
        int temp = x1;
        x1 = x2;
        x2 = temp;
    }
    
    if (y1 > y2) {
        // Swap if needed to ensure y1 <= y2
        int temp = y1;
        y1 = y2;
        y2 = temp;
    }
    
    for (int y = y1; y <= y2; y++) {
        for (int x = x1; x <= x2; x++) {
            putpixel(x, y, color, vbe_data);
        }
    }
}

// Fill the entire screen with a color
void fill_screen(unsigned long color) {
    const VbeModeInfo* vbe_data = vbe_block();

    int screen_x = vbe_data->width;
    int screen_y = vbe_data->height;
    
    for (int y = 0; y < screen_y; y++) {
        for (int x = 0; x < screen_x; x++) {
            putpixel(x, y, color, vbe_data);
        }
    }
}

#ifdef __cplusplus
}
#endif