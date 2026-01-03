#include <cstdint>

#include "util.h"

#ifndef CKERN_PAGING_H
#define CKERN_PAGING_H

namespace ckern::Paging
{
  typedef struct __attribute__((packed))
  {
    unsigned flags : 12;
    uint64_t page_dir_pointer_base : 40;
    short unused : 11;
    char nx : 1;
  } PML4Entry;

  typedef struct __attribute__((packed))
  {
    unsigned flags : 12;
    uint64_t page_dir_base : 40;
    short unused : 11;
    char nx : 1;
  } PDPEntry;

  typedef struct __attribute__((packed))
  {
    unsigned flags : 12;
    uint64_t page_table_base : 40;
    short unused : 11;
    char nx : 1;
  } PDEntry;
  
  typedef struct __attribute__((packed))
  {
    unsigned flags : 12;
    uint64_t phys_base : 40;
    short unused : 11;
    char nx : 1;
  } PTEntry;

  extern PML4Entry kern_pml4[512];

  constexpr uintptr_t PT_ADDR_MASK{0xffffffffffffffff}; // Take 40 bits after shifting by 12
  constexpr uintptr_t PT_ADDR_BASE(uintptr_t addr) { return (addr >> 12) & PT_ADDR_MASK; }

  constexpr std::size_t PAGE_SIZE{4096};

  constexpr uintptr_t VIRT_PML4_OFF(uintptr_t addr) { return (addr >> 39) & 0x1ff; }
  constexpr uintptr_t VIRT_PDP_OFF(uintptr_t addr) { return (addr >> 30) & 0x1ff; }
  constexpr uintptr_t VIRT_PD_OFF(uintptr_t addr) { return (addr >> 21) & 0x1ff; }
  constexpr uintptr_t VIRT_PT_OFF(uintptr_t addr) { return (addr >> 12) & 0x1ff; }
  constexpr uintptr_t VIRT_PHYS_OFF(uintptr_t addr) { return addr & 0xfff; }

  constexpr uintptr_t P2V(uintptr_t p_addr) { return p_addr + ckern::Util::KERN_OFFSET; }

  template<typename T>
  T *base_to_table(uintptr_t base)
  {
    return reinterpret_cast<T *>(P2V(base << 12));
  }

  constexpr uint64_t PT_PRESENT{0x1};
  constexpr uint64_t PT_RW{0x2};
  constexpr uint64_t PT_USER{0x4};

  void init_kern_paging();

  void map_page(uintptr_t phys, uintptr_t virt);

  void switch_pages(PML4Entry *new_pml4);
}

#endif
