#include "interrupts.h"

#ifndef CKERN_KEYBOARD_H
#define CKERN_KEYBOARD_H

namespace ckern::devices
{
  class Keyboard
  {
    public:

      static void install();

      static void handler(Interrupts::InterruptState state);

      static char getc();

    private:

      static inline std::array<char, 256> buffer{};

      static inline unsigned last_read_char{buffer.size() - 1};
      static inline unsigned int last_written_char{buffer.size() - 1};
  };
}

#endif
