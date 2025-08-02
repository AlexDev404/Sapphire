/*
 * Text printing functions for the Sapphire OS
 * Converted from Rust to C++
 */

#pragma once

// Include the VbeModeInfo structure definition directly
#include "vbe_mode_info.h"
#include <graphics/graphics.hpp>

// Character drawing function - should be implemented elsewhere
void drawchar(char chr, int x, int y, unsigned long fgcolor, unsigned long bgcolor);

// Print a null-terminated string to the screen
void print_text(
	const char *text,
	unsigned long fgcolor,
	unsigned long bgcolor,
	int start_x,
	int y);
// Print a null-terminated string to the screen
void print_text(
	const char *text,
	unsigned long fgcolor,
	unsigned long bgcolor,
	int start_x,
	int y,
	const VbeModeInfo *vbe_data);

// Function to print at specific row/column using character-based coordinates
void print_at(
	const char *text,
	unsigned long fgcolor,
	unsigned long bgcolor,
	int col,
	int row);

// Helper function to convert a 16-bit integer to a string
int u16_to_string(unsigned short value, unsigned char *buffer, int buffer_size);

// Function to print a 16-bit integer
void print_u16(
	unsigned short value,
	unsigned long fgcolor,
	unsigned long bgcolor,
	int x,
	int y);
