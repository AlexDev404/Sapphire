#include "stuff.h"

void circle(float x, float y, float radius, int color, const VbeModeInfo* vbe_data) {
    putpixel(x, y, color, vbe_data);
    const char radii = (unsigned char)(int)radius;
    send_debug(&radii);
}