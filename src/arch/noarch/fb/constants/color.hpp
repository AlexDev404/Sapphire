#pragma once

// Logical RGB color encoding: 0x00RRGGBB.
// putpixel translates this into the framebuffer's actual byte order.
#define RGB(r, g, b) (((r) << 16) | ((g) << 8) | (b))

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
