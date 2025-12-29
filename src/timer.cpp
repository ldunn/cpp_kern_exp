#include "timer.h"
#include "video.h"
#include "keyboard.h"

void ckern::devices::Timer::install()
{
  ckern::Interrupts::install_handler(&ckern::devices::Timer::handler, 0);
  Framebuffer::puts("Installed PIT handler.\n");
}

void ckern::devices::Timer::handler(Interrupts::InterruptState state)
{
  ticks++;
  if (ticks % 10 == 0)
  {
    const char c = Keyboard::getc();
    if (c)
      Framebuffer::putc(c);
  }
}
