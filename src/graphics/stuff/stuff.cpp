#include "stuff.h"

void circle(float x, float y, float radius, int color, const VbeModeInfo* vbe_data) {
    putpixel(x, y, color, vbe_data);
    const char radii = (unsigned char)(int)radius;
    line(x, y, x + radii, y, color, vbe_data);
    line(1, 127, 500, 800, color, vbe_data);
}