/*
 * Basic graphics primitives for the Sapphire OS
 */

#ifndef GRAPHICS_H
#define GRAPHICS_H

#include "vbe_mode_info.h"

#ifdef __cplusplus
extern "C" {
#endif

// Draw a single pixel to the framebuffer
void putpixel(int x, int y, unsigned long color, const VbeModeInfo* vbe_data);

// Draw a horizontal line
void hline(int x1, int x2, int y, unsigned long color, const VbeModeInfo* vbe_data);

// Draw a vertical line
void vline(int x, int y1, int y2, unsigned long color, const VbeModeInfo* vbe_data);

// Draw a rectangle outline
void rect(int x1, int y1, int x2, int y2, unsigned long color, const VbeModeInfo* vbe_data);

// Draw a filled rectangle
void fillrect(int x1, int y1, int x2, int y2, unsigned long color, const VbeModeInfo* vbe_data);

#ifdef __cplusplus
}
#endif

#endif /* GRAPHICS_H */