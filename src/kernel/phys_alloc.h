#include "multiboot.h"
#include "heap.h"

#include <compare>
#include <cstdint>

#ifndef CKERN_PHYS_ALLOC_H
#define CKERN_PHYS_ALLOC_H

namespace ckern::memory
{
  struct PhysAllocation
  {
    void *base{};
    memory::kunique_ptr<PhysAllocation> next{};

    PhysAllocation() = default;

    ~PhysAllocation();

    bool operator==(const PhysAllocation &) const = default;

    PhysAllocation(const PhysAllocation &) = delete;
    PhysAllocation &operator=(const PhysAllocation &) = delete;
    PhysAllocation(PhysAllocation &&);
    PhysAllocation &operator=(PhysAllocation &&);
  };

  class PhysAlloc
  {
    public:
      using BitmapType = uint64_t;
      static constexpr size_t PAGE_SIZE = 4096;
      static constexpr size_t PAGES_PER_ENTRY = sizeof(BitmapType) * 8;

      PhysAlloc() = default;

      PhysAlloc(BitmapType *bitmap, size_t bitmap_len) : bitmap(bitmap), bitmap_len(bitmap_len) {};

      ckern::memory::kunique_ptr<ckern::memory::PhysAllocation> alloc_pages(size_t size);

      void *alloc_page();

      void free(kunique_ptr<PhysAllocation> allocation);

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
