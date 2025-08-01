#include "stuff.h"

void circle(float x, float y, float radius, int color) {
    putpixel(x, y, color);
    const char radii = (unsigned char)(int)radius;
    line(x, y, x + radii, y, color);
    line(1, 127, 500, 800, color);
}