#include <kernel/graphics/primitives/line.hpp>
#include <driver/fb/fb.hpp>

// Draw a generic line
void line(int x1, int y1, int x2, int y2, uint32_t color)
{

	const float m = (y2 - y1) / (x2 - x1);

	for (int x = x1; x <= x2; x++)
	{
		const float y = (m * x) + (y1 - m);
		putpixel(x, y, color);
	}
}
