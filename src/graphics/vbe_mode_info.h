/*
 * VBE Mode Information Structure
 * Using direct primitive types
 */

#ifndef VBE_MODE_INFO_H
#define VBE_MODE_INFO_H

#ifdef __GNUC__
#define PACKED __attribute__((packed))
#else
#define PACKED
#endif

/* 
 * VBE Mode Information Block structure
 * This struct is packed to match the hardware-defined layout
 */
typedef struct PACKED {
    unsigned short mode_attributes;
    unsigned char  window_a_attributes;
    unsigned char  window_b_attributes;
    unsigned short granularity;
    unsigned short window_size;
    unsigned short segment_a;
    unsigned short segment_b;
    unsigned int   window_function_ptr;
    unsigned short pitch;
    unsigned short width;
    unsigned short height;
    unsigned char  w_char;
    unsigned char  y_char;
    unsigned char  planes;
    unsigned char  bpp;  /* bits per pixel */
    unsigned char  banks;
    unsigned char  memory_model;
    unsigned char  bank_size;
    unsigned char  image_pages;
    unsigned char  reserved0;
    unsigned char  red_mask_size;
    unsigned char  red_field_position;
    unsigned char  green_mask_size;
    unsigned char  green_field_position;
    unsigned char  blue_mask_size;
    unsigned char  blue_field_position;
    unsigned char  reserved_mask_size;
    unsigned char  reserved_field_position;
    unsigned char  direct_color_attributes;
    unsigned int   framebuffer;
    unsigned int   offscreen_mem_offset;
    unsigned short offscreen_mem_size;
    unsigned char  reserved1[206];
} VbeModeInfo;

#endif /* VBE_MODE_INFO_H */