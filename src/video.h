#include "strutil.h"

#ifndef CKERN_VIDEO_H
#define CKERN_VIDEO_H

namespace ckern
{
  class Framebuffer
  {
    enum class Color : unsigned char
    {
      Black = 0,
      Blue,
      Green,
      Cyan,
      Red,
      Purple,
      Brown,
      Gray,
      DarkGray,
      LightBlue,
      LightGreen,
      LightCyan,
      LightRed,
      LightPurple,
      Yellow,
      White
    };
    
    struct __attribute__((packed)) Character
    {
      char c;
      Color fg: 4;
      Color bg: 4;
    };

    public:

      static void set_color(Color new_fg, Color new_bg) { fg = new_fg; bg = new_bg; }

      static void move_cursor(int x, int y);

      static void cls();

      static void putc(char c);

      static void puts(const char *s);

      static void scroll(int nlines);

      template<typename ...Args>
      inline static void printf(const char *fmt, Args... args)
      {
        char buf[1024];

        sprintf(buf, fmt, args...);
        puts(buf);
      }

    private:
      inline static int cursor_x{};
      inline static int cursor_y{};

      static void write_cell(int x, int y, char c, Color fg, Color bg);

      inline static Color fg{Color::Green};
      inline static Color bg{Color::Black};

      static constexpr short CommandPort{0x3D4};
      static constexpr short DataPort{0x3D5};
      static constexpr unsigned char HighByteCommand{14};
      static constexpr unsigned char LowByteCommand{14};

      static constexpr int Rows{25};
      static constexpr int Cols{80};
      static constexpr int Size{Rows * Cols};

      inline static Character * const buffer{reinterpret_cast<Character *>(0xb8000)};
  };
}

#endif
