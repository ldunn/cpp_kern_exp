#include <array>
#include <cstdint>

#ifndef CKERN_INTERRUPTS_H
#define CKERN_INTERRUPTS_H

namespace ckern::Interrupts
{
  struct __attribute__((packed)) EncodedIDTEntry
  {
    short offset_0_15;
    short segment;
    char reserved{0};
    unsigned char type : 4;
    unsigned char zero : 1 {0};
    unsigned char dpl : 2;
    bool present : 1;
    short offset_16_31;
  };

  struct IDTEntry
  {
    uint32_t offset;
    bool present;
    unsigned char dpl;
    unsigned char type;
    short segment;

    constexpr EncodedIDTEntry encode()
    {
      EncodedIDTEntry encoded{};

      encoded.offset_0_15 = offset & 0xFFFF;
      encoded.segment = segment;
      encoded.type = type;
      encoded.dpl = dpl;
      encoded.present = present;
      encoded.offset_16_31 = (offset >> 16) & 0xFFFF;

      return encoded;
    }
  };

  struct InterruptState
  {
    uint32_t int_num;

    uint32_t edi;
    uint32_t esi;
    uint32_t ebp;
    uint32_t esp;
    uint32_t ebx;
    uint32_t edx;
    uint32_t ecx;
    uint32_t eax;

    uint32_t err_code;

    uint32_t eip;
    uint32_t cs;
    uint32_t eflags;

  };

  class IDT
  {
    public:
      static void init();
      static constexpr int entry_count{256};
    private:
      alignas(16) inline static EncodedIDTEntry entries[entry_count];
  };

  class PIC
  {
    public:
      static void init();

      static void send_EOI(int irq_num);
    
    private:

      static constexpr int MasterCommand{0x20};
      static constexpr int MasterData{0x21};
      static constexpr int SlaveCommand{0xA0};
      static constexpr int SlaveData{0xA1};

      static constexpr int CascadeIRQ{2};

      enum class Commands : unsigned char
      {
        FullInitialize = 0x11,
        ICW4Use8086 = 0x01,
        EndOfInterrupt = 0x20
      };
  };

  using Handler = void(*)(InterruptState);
  extern std::array<Handler, 16> IRQ_handlers;

  inline void install_handler(Handler h, int irq_num) { IRQ_handlers[irq_num] = h; }

  void disable();
  void enable();
}

#endif
