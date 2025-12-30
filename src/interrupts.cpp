#include "interrupts.h"
#include "gdt.h"
#include "video.h"
#include "util.h"

#include <bit>
#include <array>

std::array<ckern::Interrupts::Handler, 16> ckern::Interrupts::IRQ_handlers = {};

extern "C" void disable_interrupts();

void ckern::Interrupts::disable()
{
  disable_interrupts();
}

extern "C" void enable_interrupts();

void ckern::Interrupts::enable()
{
  enable_interrupts();
}

static std::array<const char *, 32> fault_msgs{
  "Division by zero",
  "Debug",
  "Non-maskable interrupt",
  "Breakpoint",
  "Overflow",
  "Bound range exceeded",
  "Invalid opcode",
  "No math coprocessor",
  "Double fault",
  "Reserved",
  "Invalid TSS",
  "Segment not present",
  "Stack-segment fault",
  "General protection fault",
  "Page fault",
  "Reserved",
  "Floating-point error",
  "Alignment check",
  "Machine check",
  "SIMD floating-point exception",
  "Virtualisation exception",
  "Control protection exception",
  "Reserved", "Reserved", "Reserved", "Reserved", "Reserved", "Reserved", "Reserved", "Reserved", "Reserved", "Reserved"
};

extern "C" __attribute__((noreturn)) void exception_handler(ckern::Interrupts::InterruptState state)
{
  ckern::Framebuffer::puts("[!!!] Got exception: ");
  ckern::Framebuffer::puts(fault_msgs[state.int_num]);
  ckern::Framebuffer::puts("\nMachine halted.");
  __asm__ volatile("hlt");
  while (1) {};
}

extern "C" void irq_handler(ckern::Interrupts::InterruptState state)
{
  const int irq_num = state.int_num - 32;
  if (ckern::Interrupts::IRQ_handlers[irq_num])
  {
    ckern::Interrupts::IRQ_handlers[irq_num](state);
  }
  else
  {
    ckern::Framebuffer::puts("Got unhandled IRQ: ");
    ckern::Framebuffer::putc('0' + irq_num);
    ckern::Framebuffer::putc('\n');
  }
  ckern::Interrupts::PIC::send_EOI(irq_num);
}

extern "C" void init_idt(uint16_t limit, ckern::Interrupts::EncodedIDTEntry *base);
extern void *isr_stub_table[];
void ckern::Interrupts::IDT::init()
{
  Framebuffer::puts("Initializing IDT...");

  for (int i = 0; i < 48; i++)
  {
    entries[i] = IDTEntry{
      std::bit_cast<uint32_t>(isr_stub_table[i]),
      true,
      0,
      0xF,
      GDT::Entries::Ring0Code * sizeof(GDT::EncodedGDTEntry)
    }.encode();
  }
  init_idt(sizeof(entries) - sizeof(EncodedIDTEntry), entries);

  Framebuffer::puts("done.\n");
}

void ckern::Interrupts::PIC::init()
{
  Framebuffer::puts("Configuring PICs...");

  // Remap IRQs

  // Begin re-initialization
  Util::outb(MasterCommand, static_cast<unsigned char>(Commands::FullInitialize));
  Util::outb(SlaveCommand, static_cast<unsigned char>(Commands::FullInitialize));
  
  // Set IRQ offsets - master goes to 30-37, slave 38-46
  Util::outb(MasterData, 0x20);
  Util::outb(SlaveData, 0x28);

  // Configure cascade
  Util::outb(MasterData, 1 << CascadeIRQ);
  Util::outb(SlaveData, CascadeIRQ);

  // Set 8086 mode (?)
  Util::outb(MasterData, static_cast<unsigned char>(Commands::ICW4Use8086));
  Util:: outb(SlaveData, static_cast<unsigned char>(Commands::ICW4Use8086));

  // Unmask both PICs
  Util::outb(MasterData, 0);
  Util::outb(SlaveData, 0);

  Framebuffer::puts("done.\n");
}

void ckern::Interrupts::PIC::send_EOI(int irq_num)
{
  if (irq_num >= 8)
    Util::outb(SlaveCommand, static_cast<unsigned char>(Commands::EndOfInterrupt));

  Util::outb(MasterCommand, static_cast<unsigned char>(Commands::EndOfInterrupt));
}
