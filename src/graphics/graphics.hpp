/*
 * Basic graphics primitives for the Sapphire OS
 */

#pragma once

#include "vbe_mode_info.h"
#include <devices/debug/debugcon.h>

// Define true color RGB values - no byte swapping needed since putpixel already handles BGR format
#define RGB(r, g, b) ((r << 16) | (g << 8) | b)

// Color constants with standard RGB values
#define COLOR_BLACK RGB(0, 0, 0)
#define COLOR_BLUE RGB(0, 0, 170)
#define COLOR_DARK_BLUE RGB(0, 0, 85)
#define COLOR_GREEN RGB(0, 170, 0)
#define COLOR_DARK_GREEN RGB(0, 85, 0)
#define COLOR_CYAN RGB(0, 170, 170)
#define COLOR_DARK_CYAN RGB(0, 85, 85)
#define COLOR_RED RGB(170, 0, 0)
#define COLOR_DARK_RED RGB(85, 0, 0)
#define COLOR_MAGENTA RGB(170, 0, 170)
#define COLOR_DARK_MAGENTA RGB(85, 0, 85)
#define COLOR_BROWN RGB(170, 85, 0)
#define COLOR_YELLOW RGB(170, 170, 0)
#define COLOR_LIGHT_GRAY RGB(170, 170, 170)
#define COLOR_DARK_GRAY RGB(85, 85, 85)
#define COLOR_LIGHT_BLUE RGB(85, 85, 255)
#define COLOR_LIGHT_GREEN RGB(85, 255, 85)
#define COLOR_LIGHT_CYAN RGB(85, 255, 255)
#define COLOR_LIGHT_RED RGB(255, 85, 85)
#define COLOR_LIGHT_MAGENTA RGB(255, 85, 255)
#define COLOR_WHITE RGB(255, 255, 255)

// Draw a single pixel to the framebuffer
void putpixel(int x, int y, unsigned long color);

// (overload) Draw a single pixel to the framebuffer with VbeModeInfo
void putpixel(int x, int y, unsigned long color, const VbeModeInfo *vbe_data);

// Draw a horizontal line
void hline(int x1, int x2, int y, unsigned long color);

// Draw a vertical line
void vline(int x, int y1, int y2, unsigned long color);

// Draw a generic line
void line(int x1, int y1, int x2, int y2, unsigned long color);

// Draw a rectangle outline
void rect(int x1, int y1, int x2, int y2, unsigned long color);

// Draw a filled rectangle
void fillrect(int x1, int y1, int x2, int y2, unsigned long color);

// Fill the entire screen with a color
void fill_screen(unsigned long color);

// [BINDINGS]: From stage2 bootloader
extern "C" const VbeModeInfo *vbe_block();
extern "C" const VbeModeInfo vbe_mode_block;