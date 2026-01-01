#include <cstddef>
#include <cstdint>

#ifndef CKERN_MULTIBOOT_H
#define CKERN_MULTIBOOT_H

namespace ckern
{

  constexpr uint32_t MultibootMagic{0x2BADB002};
  struct __attribute__((packed)) MultibootTag
  {
    uint32_t type;
    uint32_t size;
  };

  struct __attribute__((packed)) MultibootMemoryMap
  {
    enum class Type : uint32_t
    {
      Available = 1,
      Reserved = 2,
      ACPI = 3,
      Reserved_NVS = 4,
      Defective = 5
    };
    uint32_t size;
    uintptr_t base;
    ptrdiff_t length;
    Type type;

    MultibootMemoryMap *next() 
    { 
      auto this_bytes = reinterpret_cast<char *>(this);
      auto next_bytes = this_bytes + size + sizeof(size);
      return reinterpret_cast<MultibootMemoryMap *>(next_bytes);
    }
  };

  struct  __attribute__((packed)) MultibootInfo
  {
    uint32_t flags;
    uint32_t mem_lower;
    uint32_t mem_upper;
    uint32_t boot_device;
    uint32_t cmdline;
    uint32_t mods_count;
    uint32_t mods_addr;
    uint32_t syms[4];
    uint32_t mmap_length;
    uint32_t mmap_addr;

    bool have_mmap() { return sizeof(MultibootInfo) + (flags >> 6 & 0x1);}
    MultibootMemoryMap *get_mmap() { return reinterpret_cast<MultibootMemoryMap *>(mmap_addr); }
  };
}

#endif
