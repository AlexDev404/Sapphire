#include <driver/fb/kprint.hpp>
#include <kernel/graphics/color.hpp>

// Simple cursor tracking (character grid position)
static int g_cursor_col = 0;
static int g_cursor_row = 0;

void kprint(const char *str)
{
	draw_string_at(str, COLOR_WHITE, COLOR_BLACK, g_cursor_col, g_cursor_row);

	// Advance cursor based on string content
	for (const char *p = str; *p; p++)
	{
		if (*p == '\n')
		{
			g_cursor_col = 0;
			g_cursor_row++;
		}
		else if (*p == '\r')
		{
			g_cursor_col = 0;
		}
		else if (*p == '\t')
		{
			g_cursor_col += 4;
		}
		else if (*p == '\b')
		{
			// Can't be less than 0
			g_cursor_col = g_cursor_col <= 0 ? 0 : g_cursor_col - 1;
		}
		else
		{
			g_cursor_col++;
		}
	}
}

void kprint_at(const char *str, int col, int row)
{
	g_cursor_col = col;
	g_cursor_row = row;
	kprint(str);
}

void kputc(char c)
{
	char buf[2] = {c, '\0'};
	kprint(buf);
}

// Integer-to-string conversion helper
static char *convert(unsigned int num, int base)
{
	static char representation[] = "0123456789ABCDEF";
	static char buffer[50];
	char *ptr = &buffer[49];
	*ptr = '\0';

	do
	{
		*--ptr = representation[num % base];
		num /= base;
	} while (num != 0);

	return ptr;
}

// Source - https://stackoverflow.com/a/28581294
// Posted by vishal, modified by community. See post 'Timeline' for change history
// Retrieved 2026-05-06, License - CC BY-SA 3.0
// (Heavily modified to fix bugs)
void kprintf(const char *format, ...)
{
	va_list arg;
	va_start(arg, format);

	for (const char *p = format; *p != '\0'; p++)
	{
		if (*p != '%')
		{
			kputc(*p);
			continue;
		}

		p++; // skip '%'
		if (*p == '\0')
			break;

		switch (*p)
		{
		case 'c':
		{
			int c = va_arg(arg, int);
			kputc((char)c);
			break;
		}
		case 'd':
		{
			int val = va_arg(arg, int);
			if (val < 0)
			{
				kputc('-');
				val = -val;
			}
			kprint(convert((unsigned int)val, 10));
			break;
		}
		case 'u':
		{
			unsigned int val = va_arg(arg, unsigned int);
			kprint(convert(val, 10));
			break;
		}
		case 'o':
		{
			unsigned int val = va_arg(arg, unsigned int);
			kprint(convert(val, 8));
			break;
		}
		case 's':
		{
			const char *s = va_arg(arg, const char *);
			kprint(s ? s : "(null)");
			break;
		}
		case 'x':
		{
			unsigned int val = va_arg(arg, unsigned int);
			kprint(convert(val, 16));
			break;
		}
		case '%':
		{
			kputc('%');
			break;
		}
		default:
			// Unknown format specifier — print as-is
			kputc('%');
			kputc(*p);
			break;
		}
	}

	va_end(arg);
}
