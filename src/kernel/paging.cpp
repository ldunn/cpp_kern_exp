#include "paging.h"
#include "phys_alloc.h"
#include "util.h"
#include <bit>

alignas(ckern::Paging::PAGE_SIZE) ckern::Paging::PML4Entry ckern::Paging::kern_pml4[512];

void ckern::Paging::init_kern_paging()
{
  Util::memset(ckern::Paging::kern_pml4, 0, PAGE_SIZE);
}

void ckern::Paging::map_page(uintptr_t phys, uintptr_t virt)
{
  const auto pml4_off_id = VIRT_PML4_OFF(virt);
  const auto pdp_off_id = VIRT_PDP_OFF(virt);
  const auto pd_off_id = VIRT_PD_OFF(virt);

  if ((kern_pml4[pml4_off_id].flags & PT_PRESENT) != true)
  {
    auto new_pdp = reinterpret_cast<uintptr_t>(memory::phys_alloc.alloc_page());
    Util::memset(reinterpret_cast<void *>(new_pdp), 0, PAGE_SIZE);
    kern_pml4[pml4_off_id] = {PT_PRESENT | PT_RW, PT_ADDR_BASE(new_pdp), 0, 0};
  }

  const auto pdp = base_to_table<PDPEntry>(kern_pml4[pml4_off_id].page_dir_pointer_base);

  if ((pdp[pdp_off_id].flags & PT_PRESENT) != true)
  {
    auto new_pd = reinterpret_cast<uintptr_t>(memory::phys_alloc.alloc_page());
    Util::memset(reinterpret_cast<void *>(new_pd), 0, PAGE_SIZE);
    pdp[pdp_off_id] = {PT_PRESENT | PT_RW, PT_ADDR_BASE(new_pd), 0, 0};
  }

  const auto pd = base_to_table<PDEntry>(pdp[pdp_off_id].page_dir_base);

  if ((pd[pd_off_id].flags & PT_PRESENT) != true)
  {
    auto new_pt = reinterpret_cast<uintptr_t>(memory::phys_alloc.alloc_page());
    Util::memset(reinterpret_cast<void *>(new_pt), 0, PAGE_SIZE);
    pd[pd_off_id] = {PT_PRESENT | PT_RW, PT_ADDR_BASE(new_pt), 0, 0};
  }

  const auto pt = base_to_table<PTEntry>(pd[pd_off_id].page_table_base);

  pt[VIRT_PT_OFF(virt)] = PTEntry{PT_PRESENT | PT_RW, PT_ADDR_BASE(phys), 0, 0};
}

void ckern::Paging::switch_pages(PML4Entry *new_pml4)
{
  uintptr_t pml4_phys =(-1 * std::bit_cast<int64_t>(ckern::Util::KERN_OFFSET)) + reinterpret_cast<ptrdiff_t>(new_pml4);
  asm volatile(
    "movq %0, %%cr3"
    :
    : "r"(pml4_phys)
    : "memory"
  );
}
