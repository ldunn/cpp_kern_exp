#ifndef LOADER_PAGING_H
#define LOADER_PAGING_H

#define PML4_ADDR 0x1000
#define PDPT_ADDR 0x2000
#define PDT_ADDR 0x3000
#define PT_ADDR 0x4000
#define PT2_ADDR 0x5000

#define PT_ADDR_MASK 0xffffffffffffffff // Take 40 bits after shifting by 12

#define PT_ADDR_BASE(x) (((unsigned long long)x >> 12) & PT_ADDR_MASK)

#define PAGE_SIZE 4096

#define VIRT_PML4_OFF(x) ((x >> 39) & 0x1ff)
#define VIRT_PDP_OFF(x) ((x >> 30) & 0x1ff)
#define VIRT_PD_OFF(x) ((x >> 21) & 0x1ff)
#define VIRT_PT_OFF(x) ((x >> 12) & 0x1ff)
#define VIRT_PHYS_OFF(x) (x & 0xfff)

#define PT_PRESENT 0x1
#define PT_RW 0x2

typedef struct __attribute__((packed))
{
  unsigned flags : 12;
  unsigned long long page_dir_pointer_base : 40;
  short unused : 11;
  char nx : 1;
} pml4_t;

typedef struct __attribute__((packed))
{
  unsigned flags : 12;
  unsigned long long page_dir_base : 40;
  short unused : 11;
  char nx : 1;
} pdpe_t;

typedef struct __attribute__((packed))
{
  unsigned flags : 12;
  unsigned long long page_table_base : 40;
  short unused : 11;
  char nx : 1;
} pde_t;

typedef struct __attribute__((packed))
{
  unsigned flags : 12;
  unsigned long long phys_base : 40;
  short unused : 11;
  char nx : 1;
} pte_t;

void init_pt();

#endif
