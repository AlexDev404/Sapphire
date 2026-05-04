#include <driver/fb/primitives/string.hpp>
#include <kernel/graphics/color.hpp>

void kprint(const char *str) { draw_string(str, COLOR_WHITE, COLOR_BLACK, 0, 0); }
void kprint_at(const char *str, int col, int row) {
    draw_string_at(str, COLOR_WHITE, COLOR_BLACK, col, row);
}
void kputc(char c)           { draw_string(&c, COLOR_WHITE, COLOR_BLACK, 0, 0); }