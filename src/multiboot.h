#include <cstdint>

#ifndef CKERN_MULTIBOOT_H
#define CKERN_MULTIBOOT_H

namespace ckern
{

  constexpr uint32_t MultibootMagic{0x2BADB002};
  struct MultibootTag
  {
    uint32_t type;
    uint32_t size;
  };

  struct MultibootInfo
  {
    uint32_t flags;
    uint32_t mem_lower;
    uint32_t mem_upper;
    uint32_t boot_device;
    uint32_t cmdline;
    uint32_t mods_count;
    uint32_t mods_addr;
  };
}

#endif
