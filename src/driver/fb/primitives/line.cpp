#include <driver/fb/primitives/line.hpp>
#include <driver/fb/fb.hpp>

static inline int iabs(int x) { return x < 0 ? -x : x; }

// Draw a generic line
void line(int x1, int y1, int x2, int y2, uint32_t color) {
    int dx =  iabs(x2 - x1), sx = x1 < x2 ? 1 : -1;
    int dy = -iabs(y2 - y1), sy = y1 < y2 ? 1 : -1;
    int err = dx + dy;
    while (true) {
        putpixel(x1, y1, color);
        if (x1 == x2 && y1 == y2) break;
        int e2 = 2 * err;
        if (e2 >= dy) { err += dy; x1 += sx; }
        if (e2 <= dx) { err += dx; y1 += sy; }
    }
}