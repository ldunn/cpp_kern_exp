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

  struct __attribute__((packed)) TSSFlags
  {
    unsigned char type : 4;
    bool res : 1{false};
    unsigned char dpl : 2{0};
    bool present : 1{true};
  };

  struct __attribute__((packed)) EncodedGDTEntry
  {
    short limit_0_15;
    short base_0_15;
    unsigned char base_16_23;
    union 
    {
      Access access{};
      TSSFlags tss_flags;
    };
    unsigned char limit_16_19: 4;
    unsigned char flags : 4;
    unsigned char base_24_31;
  };
  static_assert(sizeof(EncodedGDTEntry) == 8);

  struct GDTEntry 
  {
    void *base{};
    unsigned int limit: 20{};
    Access access;
    Flags flags{};

    constexpr EncodedGDTEntry encode()
    {
      auto base_int = std::bit_cast<std::uintptr_t>(base);
      EncodedGDTEntry encoded{};
      encoded.limit_0_15 = limit & 0xFFFF;
      encoded.base_0_15 = base_int & 0xFFFF;
      encoded.base_16_23 = (base_int >> 16) & 0xFF;
      encoded.access = access;
      encoded.limit_16_19 = (limit >> 16) & 0x0F;
      encoded.flags = flags.to_bits();
      encoded.base_24_31 = (base_int >> 24) & 0xFF;

      return encoded;
    }
  };

  struct __attribute__((packed)) TSS
  {
    uint32_t reserved0{};
    void *rsp0{};
    void *rsp1{};
    void *rsp2{};
    uint64_t reserved1{};
    void *ists[8]{};
    uint16_t reserved[3]{};
    uint16_t iopb{sizeof(TSS)};
  };

  struct TSSDescriptor
  {
    void *base{};
    static constexpr unsigned int limit{sizeof(TSS) - 1};
    static constexpr TSSFlags flags{0x9, false, 0, true};

    // Encode the low 64 bits of the TSS Descriptor
    constexpr EncodedGDTEntry encode_low()
    {
      EncodedGDTEntry encoded{};
      auto base_int = std::bit_cast<std::uintptr_t>(base);

      encoded.limit_0_15 = limit & 0xFFFF;
      encoded.base_0_15 = base_int & 0xFFFF;
      encoded.base_16_23 = (base_int >> 16) & 0xFF;
      encoded.tss_flags =  flags;
      encoded.limit_16_19 = (limit >> 16) & 0x0F;
      encoded.flags = 1 << 3;
      encoded.base_24_31 = (base_int >> 24) & 0xFF;
      return encoded;
    }

    // Encode the high 64 bits of the TSS descriptor - just the high bits of the base address, with zeros above
    constexpr EncodedGDTEntry encode_high()
    {
      EncodedGDTEntry encoded{};
      auto base_int = std::bit_cast<std::uintptr_t>(base);

      uint64_t encoded_int = (base_int >> 32) & 0xFFFFFFFF;
      encoded = std::bit_cast<EncodedGDTEntry>(encoded_int);
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
    static constexpr int TSS1 = 5;
    static constexpr int TSS2 = 6;
  };

  class GDT
  {
    public:
      static void init();
      static constexpr int entry_count{7};
    private:
      alignas(16) inline static EncodedGDTEntry entries[entry_count];
  };
}

#endif
