#include "phys_alloc.h"
#include "util.h"
#include "heap.h"

#include <utility>

extern "C" char _kernel_start;
extern "C" char _kernel_end;

using PhysAlloc = ckern::memory::PhysAlloc;

static PhysAlloc::BitmapType bootstrap_bitmap[128 * 1024 * 1024 / PhysAlloc::PAGE_SIZE / PhysAlloc::PAGES_PER_ENTRY]{};

PhysAlloc ckern::memory::phys_alloc;

void ckern::memory::init_phys_alloc()
{
  phys_alloc = ckern::memory::PhysAlloc(bootstrap_bitmap, ckern::Util::arr_sizeof(bootstrap_bitmap));
}

void ckern::memory::PhysAlloc::fill_from_multiboot(ckern::MultibootInfo *mb_info)
{
  assert(mb_info->have_mmap());

  // Mark the lowest 2MiB allocated as a baseline - we will deallocate this region later on, but the bootstrap paging structures currently
  // live in the 1MiB-2MiB region
  for (uintptr_t phys_addr = 0; phys_addr < 2 * 1024 * 1024; phys_addr += PAGE_SIZE)
  {
    mark_page_filled(reinterpret_cast<void *>(phys_addr));
  }

  // Mark the kernel image as allocated
  auto kern_addr = &_kernel_start;
  while (kern_addr < &_kernel_end)
  {
    mark_page_filled(kern_addr - ckern::Util::KERN_OFFSET);
    kern_addr += PAGE_SIZE;
  }

  // TODO actually use the multiboot memory map
}

void *ckern::memory::PhysAlloc::alloc_page()
{
  size_t bitmap_idx = 0;
  while (bitmap[bitmap_idx] == static_cast<BitmapType>(-1))
  {
    bitmap_idx++;
  }

  assert(bitmap_idx < bitmap_len);

  const auto bit = ckern::Util::first_unset_bit(bitmap[bitmap_idx]);
  BitmapType set_bit = 1 << bit;
  bitmap[bitmap_idx] |= set_bit;

  return reinterpret_cast<void *>(bitmap_idx * PAGES_PER_ENTRY * PAGE_SIZE + bit * PAGE_SIZE);
}

ckern::memory::kunique_ptr<ckern::memory::PhysAllocation> ckern::memory::PhysAlloc::alloc_pages(size_t size)
{
  size_t allocated_bytes{};
  auto allocation = ckern::memory::make_kunique<PhysAllocation>();
  auto curr_allocation = allocation.get();
  size_t bitmap_idx = 0;
  while(allocated_bytes < size)
  {
    while (bitmap[bitmap_idx] == static_cast<BitmapType>(-1))
    {
      bitmap_idx++;
    }

    assert(bitmap_idx < bitmap_len);

    const auto bit = ckern::Util::first_unset_bit(bitmap[bitmap_idx]);
    BitmapType set_bit = 1 << bit;
    bitmap[bitmap_idx] |= set_bit;

    curr_allocation->base = reinterpret_cast<void *>(bitmap_idx * PAGES_PER_ENTRY * PAGE_SIZE + bit * PAGE_SIZE);
    allocated_bytes += PAGE_SIZE;

    if (allocated_bytes >= size) { break; }

    auto next_allocation = ckern::memory::make_kunique<PhysAllocation>();
    curr_allocation->next = std::move(next_allocation);
    curr_allocation = next_allocation.get();
  }
  
  return allocation;
}

void ckern::memory::PhysAlloc::mark_page_filled(void *page)
{
  const auto page_idx = reinterpret_cast<uintptr_t>(page) / PAGE_SIZE;
  const auto bitmap_idx = page_idx / PAGES_PER_ENTRY;
  const auto bitmap_bit = page_idx % PAGES_PER_ENTRY;

  const BitmapType set_bit = 1 << bitmap_bit;

  bitmap[bitmap_idx] |= set_bit;
}

void ckern::memory::PhysAlloc::free(kunique_ptr<PhysAllocation> allocation)
{
  if (!allocation) { return; }
  if (!allocation->base) { return; }

  free_page(allocation->base);
  
  if(allocation->next)
    free(std::move(allocation->next));
}

void ckern::memory::PhysAlloc::free_page(void *page)
{  
  auto page_idx = reinterpret_cast<uintptr_t>(page) / PAGE_SIZE;
  const auto bitmap_idx = page_idx / PAGES_PER_ENTRY;
  const auto bitmap_bit = page_idx % PAGES_PER_ENTRY;

  const BitmapType unset_bit = ~(1 << bitmap_bit);
  
  bitmap[bitmap_idx] &= unset_bit;
}

ckern::memory::PhysAllocation::~PhysAllocation()
{
  phys_alloc.free(this);
}

ckern::memory::PhysAllocation::PhysAllocation(PhysAllocation &&o) : 
  base(std::exchange(o.base, nullptr)),
  next(std::exchange(o.next, nullptr))
{}

ckern::memory::PhysAllocation &ckern::memory::PhysAllocation::operator=(PhysAllocation &&o)
{
  if (*this == o) { return *this; }

  base = std::exchange(o.base, nullptr);
  next = std::exchange(o.next, nullptr);

  return *this;
}
