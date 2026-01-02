#include "video.h"

#include <cstddef>
#include <cstdint>

#ifndef CKERN_HEAP_H
#define CKERN_HEAP_H

namespace ckern::memory
{
  struct HeapAllocation
  {
    static constexpr uint32_t expected_magic{0xDEADCAFE};
    uint32_t magic{expected_magic};
    uint32_t used{};
    HeapAllocation* prev{};
    HeapAllocation* next{};
    size_t size{};
    char data[];

    // Get the number of available bytes between either the start of this allocation's data (if used = false) or the end (if used = true)
    // and the beginning of the next allocation's header
    inline size_t bytes_available()
    {
      if (!used)
        return reinterpret_cast<uintptr_t>(next) - reinterpret_cast<uintptr_t>(&data[0]);
      else
        return reinterpret_cast<uintptr_t>(next) - reinterpret_cast<uintptr_t>(&data[size]);
    }

    void dump()
    {
      ckern::Framebuffer::printf("Data start: 0x%x Size: %d Used: %d\n", reinterpret_cast<uintptr_t>(&data[0]), size, used);
    }
  };

  class Heap
  {
    public:

      Heap(uintptr_t start_addr);

      void *alloc(size_t size);

      void free(void *ptr);

      void dump();
    
    private:
      uintptr_t start_addr{};
      size_t total_allocated{};

      void add_page();

      void *alloc_heap_end(HeapAllocation *last, size_t size);
  };

  Heap &init_kern_heap();

  constexpr uintptr_t KHEAP_START_ADDR = 0xfffffc0000000000; // -4TB
}

#endif
