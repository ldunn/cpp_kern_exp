#include "paging.h"
#include "util.h"

extern pml4_t bootstrap_PML4T[512];
pml4_t bootstrap_PML4T[512];

extern pdpe_t bootstrap_PDPT[512];
pdpe_t bootstrap_PDPT[512];

extern pde_t bootstrap_PD[512];
pde_t bootstrap_PD[512];

extern pte_t bootstrap_PT[64][512];
pte_t bootstrap_PT[64][512];

void init_pt()
{
  memset((char *)bootstrap_PML4T, 0, PAGE_SIZE);
  memset((char *)bootstrap_PDPT, 0, PAGE_SIZE);
  memset((char *)bootstrap_PD, 0, PAGE_SIZE);
  memset((char *)bootstrap_PT, 0, 64 * PAGE_SIZE);

  // Identity map the first 128 MiB and map the same 128MiB into the kernel text region
  unsigned long long virt_addr_id = 0;
  unsigned long long virt_addr_high = 0xffffffff80000000; // -2GB
  unsigned long long phys_addr = 0;

  unsigned x = VIRT_PT_OFF(4096);

  while (phys_addr < 128 * 1024 * 1024)
  {
    const unsigned pml4_off_id = VIRT_PML4_OFF(virt_addr_id);
    const unsigned pdp_off_id = VIRT_PDP_OFF(virt_addr_id);
    const unsigned pd_off_id = VIRT_PD_OFF(virt_addr_id);

    const unsigned pml4_off_high = VIRT_PML4_OFF(virt_addr_high);
    const unsigned pdp_off_high = VIRT_PDP_OFF(virt_addr_high);
    const unsigned pd_off_high = VIRT_PD_OFF(virt_addr_high);

    const unsigned pt_off = VIRT_PT_OFF(virt_addr_id);

    bootstrap_PML4T[pml4_off_id] = (pml4_t){
      .flags = PT_PRESENT | PT_RW,
      .page_dir_pointer_base = PT_ADDR_BASE(&bootstrap_PDPT[pdp_off_id])
    };

    bootstrap_PDPT[pdp_off_id] = (pdpe_t){
      .flags = PT_PRESENT | PT_RW,
      .page_dir_base = PT_ADDR_BASE(&bootstrap_PD[pd_off_id])
    };

    bootstrap_PD[pd_off_id] = (pde_t){
      .flags = PT_PRESENT | PT_RW,
      .page_table_base = PT_ADDR_BASE(&bootstrap_PT[pd_off_id][0])
    };

    bootstrap_PML4T[pml4_off_high] = (pml4_t){
      .flags = PT_PRESENT | PT_RW,
      .page_dir_pointer_base = PT_ADDR_BASE(&bootstrap_PDPT[pdp_off_high])
    };

    bootstrap_PDPT[pdp_off_high] = (pdpe_t){
      .flags = PT_PRESENT | PT_RW,
      .page_dir_base = PT_ADDR_BASE(&bootstrap_PD[pd_off_high])
    };

    bootstrap_PD[pd_off_high] = (pde_t){
      .flags = PT_PRESENT | PT_RW,
      .page_table_base = PT_ADDR_BASE(&bootstrap_PT[pd_off_id][0])
    };

    
    bootstrap_PT[pd_off_id][pt_off] = (pte_t){
      .flags = PT_PRESENT | PT_RW,
      .phys_base = PT_ADDR_BASE(phys_addr)
    };

    phys_addr += PAGE_SIZE;
    virt_addr_id += PAGE_SIZE;
    virt_addr_high += PAGE_SIZE;
  }
}
