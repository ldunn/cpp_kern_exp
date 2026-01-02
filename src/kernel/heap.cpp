#include "heap.h"
#include "phys_alloc.h"
#include "paging.h"

#include <new>

ckern::memory::Heap &ckern::memory::init_kern_heap()
{
  static ckern::memory::Heap kheap(ckern::memory::KHEAP_START_ADDR);
  return kheap;
}

ckern::memory::Heap::Heap(uintptr_t start_addr) : start_addr(start_addr)
{
  add_page();
  HeapAllocation *base = reinterpret_cast<HeapAllocation *>(start_addr);
  *base = HeapAllocation{};
}

void ckern::memory::Heap::add_page()
{
  auto new_page = ckern::memory::phys_alloc.alloc_page();
  ckern::Paging::map_page(reinterpret_cast<uintptr_t>(new_page), start_addr + total_allocated);
  total_allocated += ckern::Paging::PAGE_SIZE;
}

void ckern::memory::Heap::dump()
{
  ckern::Framebuffer::printf("=========== HEAP ALLOCATIONS ===========\n");

  HeapAllocation *allocation = reinterpret_cast<HeapAllocation *>(start_addr);
  while (allocation->next)
  {
    allocation->dump();
    allocation = allocation->next;
  }
  allocation->dump();
}

void *ckern::memory::Heap::alloc(size_t size)
{
  HeapAllocation *allocation = reinterpret_cast<HeapAllocation *>(start_addr);
  while (allocation->next)
  {
    // If we have a previous allocation that is now free and is large enough, repurpose it
    if (!allocation->used && allocation->bytes_available() >= size)
    {
      allocation->size = size;
      allocation->used = true;
      return &allocation->data[0];
    }
    // If we have a previous allocation with some space above it that's large enough, squeeze in
    else if (allocation->used && (allocation->bytes_available() >= size + sizeof(HeapAllocation)))
    {
      auto next = allocation->next;
      auto *new_alloc = reinterpret_cast<HeapAllocation *>(&allocation->data[allocation->size]);
      new_alloc = new (new_alloc) HeapAllocation();

      new_alloc->size = size;
      new_alloc->used = 1;
      
      // Insert into the linked list
      next->prev = new_alloc;
      allocation->next = new_alloc;
      new_alloc->prev = allocation;
      new_alloc->next = next;

      return &new_alloc->data[0];
    }

    allocation = allocation->next;
  }

  // If we get to the end of the heap, allocate off the end
  return alloc_heap_end(allocation, size);
}

void *ckern::memory::Heap::alloc_heap_end(HeapAllocation *last, size_t size)
{
  // How much space do we have above the last allocation that's still backed?
  const auto last_end_offset = reinterpret_cast<uintptr_t>(last) + last->size + sizeof(*last) - start_addr;
  const auto remaining = total_allocated - last_end_offset;
  const auto required = size + sizeof(HeapAllocation);
  if (required <= remaining)
  {
    HeapAllocation *new_last = reinterpret_cast<HeapAllocation *>(start_addr + last_end_offset);
    new_last = new (new_last) HeapAllocation();

    new_last->size = size;
    new_last->prev = last;
    new_last->used = true;
    
    last->next = new_last;
    return reinterpret_cast<void *>(new_last + 1);
  }

  return nullptr;
}

void ckern::memory::Heap::free(void *ptr)
{
  HeapAllocation *allocation = reinterpret_cast<HeapAllocation *>(ptr) - 1;
  allocation->used = false;
}
