#include "multiboot.h"

#include <cstdint>

#ifndef CKERN_PHYS_ALLOC_H
#define CKERN_PHYS_ALLOC_H

namespace ckern::memory
{
  class PhysAlloc
  {
    public:
      using BitmapType = uint64_t;
      static constexpr size_t PAGE_SIZE = 4096;
      static constexpr size_t PAGES_PER_ENTRY = sizeof(BitmapType) * 8;

      PhysAlloc() = default;

      PhysAlloc(BitmapType *bitmap, size_t bitmap_len) : bitmap(bitmap), bitmap_len(bitmap_len) {};

      void *alloc_page();

      void free_page(void *page);

      void fill_from_multiboot(ckern::MultibootInfo *mb_info);

    private:

      void mark_page_filled(void *page);

      BitmapType *bitmap;
      size_t bitmap_len;
  };

  extern PhysAlloc phys_alloc;

  void init_phys_alloc();
}

#endif
