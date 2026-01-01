#include "interrupts.h"

#ifndef CKERN_TIMER_H
#define CKERN_TIMER_H

namespace ckern::devices
{
  class Timer
  {
    public:
      static void install();

      static void handler(Interrupts::InterruptState state);

    private:

      inline static int ticks{};
  };
}

#endif
