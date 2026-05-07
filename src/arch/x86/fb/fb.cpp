#include "fb.h"

// Implements getting the framebuffer for the x86 architecture
const FramebufferInfo *get_framebuffer_info()
{
	static FramebufferInfo fb_info;
	const VbeModeInfo *vbe_info = vbe_block();

	fb_info.width		= vbe_info->width;
	fb_info.height		= vbe_info->height;
	fb_info.pitch		= vbe_info->pitch;
	fb_info.bpp			= vbe_info->bpp;
	fb_info.framebuffer	= vbe_info->framebuffer;
	fb_info.size		= (vbe_info->pitch * vbe_info->height * (vbe_info->bpp / 8));

	return &fb_info;
}