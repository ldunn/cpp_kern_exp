#include "gdt.h"
#include "video.h"

struct
{
  uint16_t limit;
  ckern::GDT::EncodedGDTEntry *addr;
} __attribute__((packed)) gdtr;

extern "C" void init_gdt(void *gdtr);

ckern::GDT::TSS tss{};

void ckern::GDT::GDT::init()
{
  ckern::Framebuffer::puts("Initializing GDT...");

  entries[Entries::Null] = ckern::GDT::GDTEntry{}.encode();

  entries[Entries::Ring0Code] = ckern::GDT::GDTEntry{
    0x0, 
    0xFFFFF,
    {true, true, false, true, 1, 0, 1},
    {true, 0, 1}
  }.encode();
  entries[Entries::Ring0Data] = ckern::GDT::GDTEntry{
    0x0, 
    0xFFFFF,
    {true, true, false, false, 1, 0, 1},
    {true, 0, 1}
  }.encode();
  entries[Entries::Ring3Code] = ckern::GDT::GDTEntry{
    0x0, 
    0xFFFFF,
    {true, true, true, true, 1, 3, 1},
    {true, 0, 1}
  }.encode();
  entries[Entries::Ring3Data] = ckern::GDT::GDTEntry{
    0x0, 
    0xFFFFF,
    {true, true, false, false, 1, 3, 1},
    {true, 0, 1}
  }.encode();

  // TODO make a new stack for this?
  tss.rsp0 = reinterpret_cast<void *>(ckern::Util::KERN_OFFSET-8);

  auto tss_desc = ckern::GDT::TSSDescriptor{&tss};

  entries[Entries::TSS1] = tss_desc.encode_low();
  entries[Entries::TSS2] = tss_desc.encode_high();

  gdtr.limit = sizeof(entries) - 1;
  gdtr.addr = entries;
  init_gdt(&gdtr);

  ckern::Framebuffer::puts("done.\n");
}
