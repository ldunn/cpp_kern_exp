#include "util.h"
#include "video.h"

void ckern::Framebuffer::write_cell(int x, int y, char c, Color fg, Color bg)
{
  short pos = (x * Cols + y) % Size;
  buffer[pos] = Character{c, fg, bg};
}

#define FB_COMMAND_PORT 0x3D4
#define FB_DATA_PORT 0x3D5

#define FB_HIGH_BYTE_COMMAND 14
#define FB_LOW_BYTE_COMMAND 15

void ckern::Framebuffer::move_cursor(int x, int y)
{
  short pos = (x * Cols + y) % Size;
  Util::outb(CommandPort, HighByteCommand);
  Util::outb(DataPort, ((pos >> 8) & 0x00FF));
  Util::outb(CommandPort, LowByteCommand);
  Util::outb(DataPort, pos & 0x00FF);

  cursor_x = x;
  cursor_y = y;
}

void ckern::Framebuffer::cls()
{
  for (int i = 0; i < Rows; i++)
  {
    for (int j = 0; j < Cols; j++)
    {
      write_cell(i, j, ' ', fg, bg);
    }
  }
}

void ckern::Framebuffer::putc(char c)
{
  if (cursor_x >= Rows)
  {
    scroll(1);
  }
  
  if (c == '\n')
  {
    move_cursor(++cursor_x, 0);
  }
  else
  {
    write_cell(cursor_x, cursor_y++, c, fg, bg);
    move_cursor(cursor_x, cursor_y);
  }
  if (cursor_y >= Cols)
  {
    move_cursor(++cursor_x, 0);
  }
}

void ckern::Framebuffer::puts(const char *s)
{
  while (*s) 
  { 
    putc(*s++);
  }
}

void ckern::Framebuffer::scroll(int nlines)
{
  Util::memcpy(
    buffer, 
    buffer + Cols * nlines, 
    Cols * (Rows - nlines) * sizeof(buffer[0])
  );
  move_cursor(cursor_x - nlines, 0);
  for (int i = 0; i < Cols; i++) { putc(' '); }
  move_cursor(cursor_x - nlines, cursor_y);
}
