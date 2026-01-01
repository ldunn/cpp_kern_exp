#include <bit>
#include <cstdint>

#ifndef CKERN_GDT_H
#define CKERN_GDT_H

namespace ckern::GDT
{
  struct __attribute__((packed)) Flags
  {
    bool long_mode : 1 {false};
    unsigned char type : 1 {false};
    unsigned char granularity : 1 {false}; 

    constexpr unsigned char to_bits()
    {
      unsigned char bits{};
      bits |= (long_mode << 1);
      bits |= (type << 2);
      bits |= (granularity << 3);
      return bits;
    }
  };

  struct __attribute__((packed))  Access
  {
    bool accessed : 1;
    bool readwrite : 1;
    bool direc_conform : 1;
    bool exec : 1;
    unsigned char type : 1;
    unsigned char dpl : 2;
    bool present : 1;
  };

  struct __attribute__((packed)) EncodedGDTEntry
  {
    short limit_0_15;
    short base_0_15;
    unsigned char base_16_23;
    Access access;
    unsigned char limit_16_19: 4;
    unsigned char flags : 4;
    unsigned char base_24_31;
  };
  static_assert(sizeof(EncodedGDTEntry) == 8);

  struct GDTEntry 
  {
    void *base{};
    unsigned int limit: 20{};
    Access access{};
    Flags flags{};

    constexpr EncodedGDTEntry encode()
    {
      auto base_int = std::bit_cast<std::uintptr_t>(base);
      EncodedGDTEntry encoded{};
      encoded.limit_0_15 = limit & 0xFF;
      encoded.base_0_15 = base_int & 0xFF;
      encoded.base_16_23 = (base_int >> 16) & 0x0F;
      encoded.access = access;
      encoded.limit_16_19 = (limit >> 16) & 0x0F;
      encoded.flags = flags.to_bits();
      encoded.base_24_31 = (base_int >> 24) & 0x0F;

      return encoded;
    }
  };

  struct Entries
  {
    static constexpr int Null = 0;
    static constexpr int Ring0Code = 1;
    static constexpr int Ring0Data = 2;
    static constexpr int Ring3Code = 3;
    static constexpr int Ring3Data = 4;
  };

  class GDT
  {
    public:
      static void init();
      static constexpr int entry_count{5};
    private:
      alignas(16) inline static EncodedGDTEntry entries[entry_count];
  };
}

#endif
