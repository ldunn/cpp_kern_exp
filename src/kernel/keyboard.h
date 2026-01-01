#include "interrupts.h"
#include "util.h"

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

      static inline char buffer[256]{};

      static inline unsigned last_read_char{Util::arr_sizeof(buffer) - 1};
      static inline unsigned int last_written_char{Util::arr_sizeof(buffer) - 1};
  };
}

#endif
