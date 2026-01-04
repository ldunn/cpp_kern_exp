#include "paging.h"
#include "phys_alloc.h"
#include "util.h"

alignas(ckern::Paging::PAGE_SIZE) ckern::Paging::PML4Entry ckern::Paging::kern_pml4[512];

void ckern::Paging::init_kern_paging()
{
  Util::memset(ckern::Paging::kern_pml4, 0, sizeof(ckern::Paging::kern_pml4));
}

void ckern::Paging::map_page(PML4Entry *pml4, uintptr_t phys, uintptr_t virt)
{
  const auto pml4_off_id = VIRT_PML4_OFF(virt);
  const auto pdp_off_id = VIRT_PDP_OFF(virt);
  const auto pd_off_id = VIRT_PD_OFF(virt);

  if ((pml4[pml4_off_id].flags & PT_PRESENT) != true)
  {
    auto new_pdp = reinterpret_cast<uintptr_t>(memory::phys_alloc.alloc_page());
    Util::memset(reinterpret_cast<void *>(P2V(new_pdp)), 0, PAGE_SIZE);
    pml4[pml4_off_id] = {PT_PRESENT | PT_RW | PT_USER, PT_ADDR_BASE(new_pdp), 0, 0};
  }

  const auto pdp = base_to_table<PDPEntry>(pml4[pml4_off_id].page_dir_pointer_base);

  if ((pdp[pdp_off_id].flags & PT_PRESENT) != true)
  {
    auto new_pd = reinterpret_cast<uintptr_t>(memory::phys_alloc.alloc_page());
    Util::memset(reinterpret_cast<void *>(P2V(new_pd)), 0, PAGE_SIZE);
    pdp[pdp_off_id] = {PT_PRESENT | PT_RW | PT_USER, PT_ADDR_BASE(new_pd), 0, 0};
  }

  const auto pd = base_to_table<PDEntry>(pdp[pdp_off_id].page_dir_base);

  if ((pd[pd_off_id].flags & PT_PRESENT) != true)
  {
    auto new_pt = reinterpret_cast<uintptr_t>(memory::phys_alloc.alloc_page());
    Util::memset(reinterpret_cast<void *>(P2V(new_pt)), 0, PAGE_SIZE);
    pd[pd_off_id] = {PT_PRESENT | PT_RW | PT_USER, PT_ADDR_BASE(new_pt), 0, 0};
  }

  const auto pt = base_to_table<PTEntry>(pd[pd_off_id].page_table_base);

  pt[VIRT_PT_OFF(virt)] = PTEntry{PT_PRESENT | PT_RW | PT_USER, PT_ADDR_BASE(phys), 0, 0};
}

uintptr_t ckern::Paging::get_active_pages()
{
  uintptr_t pml4_phys;
  asm volatile(
    "movq %%cr3, %0" : "=r"(pml4_phys) ::
  );
  return pml4_phys;
}

void ckern::Paging::switch_pages(PML4Entry *new_pml4)
{
  uintptr_t pml4_phys = V2P(reinterpret_cast<uintptr_t>(new_pml4));
  asm volatile(
    "movq %0, %%cr3"
    :
    : "r"(pml4_phys)
    : "memory"
  );
}

uintptr_t ckern::Paging::V2P(uintptr_t virt)
{
  auto pml4_phys = get_active_pages();
  auto pml4 = reinterpret_cast<PML4Entry *>(P2V(pml4_phys));

  const auto virt_page_off = virt & 0xfff;

  const auto pml4_off = VIRT_PML4_OFF(virt);
  const auto pdp_off = VIRT_PDP_OFF(virt);
  const auto pd_off = VIRT_PD_OFF(virt);

  auto pdp = base_to_table<PDPEntry>(pml4[pml4_off].page_dir_pointer_base);
  auto pd = base_to_table<PDEntry>(pdp[pdp_off].page_dir_base);
  auto pt = base_to_table<PTEntry>(pd[pd_off].page_table_base);

  return (pt[VIRT_PT_OFF(virt)].phys_base << 12) + virt_page_off;
}
