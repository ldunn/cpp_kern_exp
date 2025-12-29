#include "interrupts.h"
#include "video.h"
#include "gdt.h"
#include "interrupts.h"
#include "timer.h"
#include "keyboard.h"

extern "C" int kmain()
{
  ckern::Interrupts::disable();

  ckern::Framebuffer::cls();
  ckern::Framebuffer::puts("Hello World\n");

  ckern::GDT::GDT::init();
  ckern::Interrupts::IDT::init();
  ckern::Interrupts::PIC::init();

  ckern::devices::Timer::install();
  ckern::devices::Keyboard::install();

  ckern::Interrupts::enable();
  
  while (1) {};

  return -1;
}
