#include <driver/fb/fb.hpp>
#include <stdint.h>

// Draw a single pixel to the framebuffer
void putpixel(int x, int y, uint32_t color)
{
	// Get the framebuffer
	const FramebufferInfo *fb = get_framebuffer_info();

	// Handle a bad framebuffer or out-of-bounds coordinates
	if ((fb == nullptr) || (x < 0 || y < 0 || x >= fb->width || y >= fb->height))
	{
		return;
	}

	// Calculate base address in the framebuffer
	uint8_t *framebuffer = reinterpret_cast<uint8_t *>(fb->framebuffer);

	// Bytes per pixel
	int bpp = fb->bpp;

	// Calculate position in framebuffer (use pitch, not width — pitch may include padding)
	int fb_offset = (y * fb->pitch) + (x * (bpp / 8));

	if (bpp == 32)
	{
		// 32 bits per pixel (BGRA — assumes VBE LFB / common QEMU layout)
		framebuffer[fb_offset + 0] = (color >> 0)  & 0xFF; // Blue
		framebuffer[fb_offset + 1] = (color >> 8)  & 0xFF; // Green
		framebuffer[fb_offset + 2] = (color >> 16) & 0xFF; // Red
		framebuffer[fb_offset + 3] = (color >> 24) & 0xFF; // Alpha
	}
	else if (bpp == 24)
	{
		// 24 bits per pixel (BGR)
		framebuffer[fb_offset + 0] = (color >> 0)  & 0xFF; // Blue
		framebuffer[fb_offset + 1] = (color >> 8)  & 0xFF; // Green
		framebuffer[fb_offset + 2] = (color >> 16) & 0xFF; // Red
	}
	else if (bpp == 16)
	{
		// 16 bits per pixel (5-6-5 RGB)
		uint16_t pixel_color =
			((((color >> 16) & 0xFF) >> 3) << 11) |  // Red   (5 bits)
			((((color >> 8)  & 0xFF) >> 2) << 5)  |  // Green (6 bits)
			((((color >> 0)  & 0xFF) >> 3));         // Blue  (5 bits)

		// Byte-by-byte store: avoids strict-aliasing UB and unaligned-access issues
		framebuffer[fb_offset + 0] = pixel_color & 0xFF;
		framebuffer[fb_offset + 1] = (pixel_color >> 8) & 0xFF;
	}
}