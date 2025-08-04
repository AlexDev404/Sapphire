/*
 * Simple PS/2 Keyboard Driver for Sapphire OS
 * Handles basic keyboard input and scan code translation
 */

#include "keyboard.hpp"

// Basic scan code to ASCII mapping for US QWERTY layout
// This is a simplified mapping for demonstration purposes
static const char scancode_to_ascii[128] = {
    0,  27, '1', '2', '3', '4', '5', '6', '7', '8',    /* 9 */
    '9', '0', '-', '=', '\b',   /* Backspace */
    '\t',           /* Tab */
    'q', 'w', 'e', 'r',    /* 19 */
    't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n', /* Enter key */
    0,          /* 29   - Control */
    'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', /* 39 */
    '\'', '`',   0,     /* Left shift */
    '\\', 'z', 'x', 'c', 'v', 'b', 'n',            /* 49 */
    'm', ',', '.', '/',   0,                /* Right shift */
    '*',
    0,  /* Alt */
    ' ',    /* Space bar */
    0,  /* Caps lock */
    0,  /* 59 - F1 key ... > */
    0,   0,   0,   0,   0,   0,   0,   0,
    0,  /* < ... F10 */
    0,  /* 69 - Num lock*/
    0,  /* Scroll Lock */
    0,  /* Home key */
    0,  /* Up Arrow */
    0,  /* Page Up */
    '-',
    0,  /* Left Arrow */
    0,
    0,  /* Right Arrow */
    '+',
    0,  /* 79 - End key*/
    0,  /* Down Arrow */
    0,  /* Page Down */
    0,  /* Insert Key */
    0,  /* Delete Key */
    0,   0,   0,
    0,  /* F11 Key */
    0,  /* F12 Key */
    0,  /* All other keys are undefined */
};

/**
 * Read scan code from keyboard data port
 * This must be called from the keyboard interrupt handler
 * to clear the interrupt condition
 */
uint8_t keyboard_read_scan_code(void) {
    // This function should use inportb(0x60) - you'll need to implement this
    // or include the appropriate header that defines inportb()
    extern uint8_t inportb(uint16_t port);
    return inportb(KEYBOARD_DATA_PORT);
}

/**
 * Convert scan code to ASCII character
 * Returns 0 for non-printable keys
 */
char scancode_to_char(uint8_t scancode) {
    // Handle key release (high bit set)
    if (scancode & 0x80) {
        return 0; // Key release - don't print anything
    }
    
    // Make sure scancode is within our table bounds
    if (scancode >= 128) {
        return 0;
    }
    
    return scancode_to_ascii[scancode];
}

/**
 * Handle a keyboard scan code
 * This should be called from your keyboard interrupt handler
 */
void keyboard_handle_scancode(uint8_t scancode) {
    char ascii_char = scancode_to_char(scancode);
    
    if (ascii_char != 0) {
        // Handle printable character
        // You can add your own logic here to:
        // - Add to keyboard buffer
        // - Echo to screen
        // - Process special keys (Enter, Backspace, etc.)
        
        // Example: Just echo the character for now
        // You'll need to implement or include these functions:
        // putchar(ascii_char);  // or your equivalent function
    }
    
    // Handle special keys that don't have ASCII equivalents
    switch (scancode) {
        case 0x1C: // Enter key
            // Handle Enter
            break;
        case 0x0E: // Backspace
            // Handle Backspace  
            break;
        case 0x39: // Space bar
            // Handle Space (already in ASCII table as ' ')
            break;
        // Add more special key handling as needed
    }
}
