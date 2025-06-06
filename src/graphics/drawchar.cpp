/*
 * Character drawing implementation for the Sapphire OS
 */

#include "drawchar.h"

// Character drawing function implementation
void drawchar(char chr, int x, int y, unsigned long fgcolor, unsigned long bgcolor, const VbeModeInfo* vbe_data) {
    // Handle invalid coordinates
    if (x < 0 || y < 0 || x >= vbe_data->width || y >= vbe_data->height) {
        return;
    }

    // Get character index in the font table
    unsigned char* char_data = (unsigned char*)&IBM_VGA_8X8[(unsigned char)chr * 8];
    
    // Calculate base address in the framebuffer
    unsigned char* framebuffer = (unsigned char*)vbe_data->framebuffer;
    
    // Set bytes per pixel based on color depth
    int bytes_per_pixel = vbe_data->bpp / 8;
    
    // Character dimensions
    const int CHAR_WIDTH = 8;
    const int CHAR_HEIGHT = 8;
    
    // Draw each row of the character
    for (int cy = 0; cy < CHAR_HEIGHT; cy++) {
        // Get the current row of pixel data
        unsigned char row = char_data[cy];
        
        // Draw each column of the character
        for (int cx = 0; cx < CHAR_WIDTH; cx++) {
            // Determine if this pixel should be foreground or background
            unsigned long color = (row & (1 << (CHAR_WIDTH - 1 - cx))) ? fgcolor : bgcolor;
            
            // Calculate position in framebuffer
            int fb_offset = ((y + cy) * vbe_data->pitch) + ((x + cx) * bytes_per_pixel);
            
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
                unsigned char gray = (
                    (((color >> 16) & 0xFF) * 30 + 
                     ((color >> 8) & 0xFF) * 59 + 
                     ((color >> 0) & 0xFF) * 11) / 100
                );
                framebuffer[fb_offset] = gray;
            }
        }
    }
}