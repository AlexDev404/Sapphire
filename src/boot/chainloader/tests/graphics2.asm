vbe_info:

    .signature db "VBE2";   // must be "VESA" to indicate valid VBE support
    .version resw 1;            // VBE version; high byte is major version, low byte is minor version
    .oem resd 1;            // segment:offset pointer to OEM
    .capabilities resd 1;       // bitfield that describes card capabilities
    .video_modes resd 1;        // segment:offset pointer to list of supported video modes
    .video_memory resw 1;       // amount of video memory in 64KB blocks
    .software_rev resw 1;       // software revision
    .vendor resd 1;         // segment:offset to card vendor string
    .product_name resd 1;       // segment:offset to card model name
    .product_rev resd 1;        // segment:offset pointer to product revision
    .reserved resb 222;     // reserved for future expansion
    .oem_data resb 256;     // OEM BIOSes store their strings in this area

mode_info:

    .attributes resw 1;     // deprecated, only bit 7 should be of interest to you, and it indicates the mode supports a linear frame buffer.
    .window_a resb 1;           // deprecated
    .window_b resb 1;           // deprecated
    .granularity resw 1;        // deprecated; used while calculating bank numbers
    .window_size resw 1;
    .segment_a resw 1;
    .segment_b resw 1;
    .win_func_ptr resd 1;       // deprecated; used to switch banks from protected mode without returning to real mode
    .pitch resw 1;          // number of bytes per horizontal line
    .width resw 1;          // width in pixels
    .height resw 1;         // height in pixels
    .w_char resb 1;         // unused...
    .y_char resb 1;         // ...
    .planes resb 1;
    .bpp resb 1;            // bits per pixel in this mode
    .banks resb 1;          // deprecated; total number of banks in this mode
    .memory_model resb 1;
    .bank_size resb 1;      // deprecated; size of a bank, almost always 64 KB but may be 16 KB...
    .image_pages resb 1;
    .reserved0 resb 1;

    .red_mask resb 1;
    .red_position resb 1;
    .green_mask resb 1;
    .green_position resb 1;
    .blue_mask resb 1;
    .blue_position resb 1;
    .reserved_mask resb 1;
    .reserved_position resb 1;
    .direct_color_attributes resb 1;

    .framebuffer resd 1;        // physical address of the linear frame buffer; write here to draw to the screen
    .off_screen_mem_off resd 1;
    .off_screen_mem_size resw 1;    // size of memory in the framebuffer but not being displayed on the screen
    .reserved1 resb 206;

enable_vesa:

vbe_get_info:

    mov ah, 4Fh;        Super VGA support
    mov al, 00h;        Return Super VGA information
    mov di, vbe_info;   Pointer to buffer

    int 0x10;

    cmp ax, 0x4F                ; BIOS doesn't support VBE?
    jne error

get_mode_info:

    mov ax, 4F01h;        Return mode information
    mov cx, 0x101;[vbe_info.video_modes]; first mode
    mov di, mode_info;  Pointer to buffer

    int 0x10;

    cmp ax, 0x4F                ; BIOS doesn't support VBE?
    jne error

set_mode:

    mov ah, 0
    mov ax, 0x4F02
    mov ebx, [vbe_info.video_modes]; estore de modes pointer at ebx to can access as a adress
    mov bx, [ebx+8]; 8/2 = 4th mode in the mode array!!!!!!!

    int 0x10

draw:

    ;Assume first window is valid 
    mov ax, WORD [es:mode_info + 08h]
    mov es, ax

    ;Example of how to change the window 
    mov ax, 4f05h
    xor bx, bx
    mov dx, 5       ;This is granularity units
    int 10h

    ;fist atempt
    mov edi, [mode_info.framebuffer];   framebuffer
    add edi, 180050;                        pixel_offset = y * pitch + ( x * ( bpp/8 )) + framebuffer;
    mov al,0x0F;                        the color of the pixel
    mov [edi], al

    mov si, msg;
    call print

    jmp $

msg db "finish", 0