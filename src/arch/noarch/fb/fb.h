#pragma once
#include <stdint.h>

// Generic framebuffer interface for noarch architecture
typedef struct {
    unsigned short pitch;
    unsigned short width;
    unsigned short height;
    unsigned char  bpp;          /* bits per pixel */
    uint32_t       size;         /* framebuffer size in bytes */
    uintptr_t      framebuffer;  /* address of the linear framebuffer */
} FramebufferInfo;

// Get the framebuffer
const FramebufferInfo *get_framebuffer_info();