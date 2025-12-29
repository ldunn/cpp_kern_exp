#include "gdt.h"
#include "video.h"
extern "C" void init_gdt(short limit, ckern::GDT::EncodedGDTEntry *base);

void ckern::GDT::GDT::init()
{
  ckern::Framebuffer::puts("Initializing GDT...");

  entries[Entries::Null] = ckern::GDT::GDTEntry{}.encode();

  entries[Entries::Ring0Code] = ckern::GDT::GDTEntry{
    0x0, 
    0xFFFFF,
    {true, true, true, true, 1, 0, 1},
    {false, 1, 1}
  }.encode();
  entries[Entries::Ring0Data] = ckern::GDT::GDTEntry{
    0x0, 
    0xFFFFF,
    {true, true, false, false, 1, 0, 1},
    {false, 1, 1}
  }.encode();
  entries[Entries::Ring3Code] = ckern::GDT::GDTEntry{
    0x0, 
    0xFFFFF,
    {true, true, true, true, 1, 3, 1},
    {false, 1, 1}
  }.encode();
  entries[Entries::Ring3Data] = ckern::GDT::GDTEntry{
    0x0, 
    0xFFFFF,
    {true, true, false, false, 1, 3, 1},
    {false, 1, 1}
  }.encode();
  init_gdt(sizeof(entries), entries);

  ckern::Framebuffer::puts("done.\n");
}
