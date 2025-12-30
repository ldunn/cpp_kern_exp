#include "interrupts.h"
#include "video.h"
#include "gdt.h"
#include "interrupts.h"
#include "timer.h"
#include "keyboard.h"
#include "multiboot.h"
#include "strutil.h"

void foo() { return; }

extern "C" int kmain(uint32_t mb_magic, ckern::MultibootInfo *mb_info)
{
  ckern::Interrupts::disable();
  ckern::Framebuffer::cls();
  ckern::Framebuffer::puts("Hello World\n");
  ckern::Interrupts::IDT::init();
  ckern::Interrupts::PIC::init();
  ckern::Interrupts::enable();
  ckern::devices::Timer::install();
  ckern::devices::Keyboard::install();
  while (1) {};

  ckern::GDT::GDT::init();







  if (mb_magic == ckern::MultibootMagic)
  {
    ckern::Framebuffer::printf("Found correct multiboot magic: 0x%x\n", mb_magic);
  }
  
  
  while (1) {};

  return -1;
}
