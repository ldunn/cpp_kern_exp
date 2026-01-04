#include "process.h"
#include "util.h"
#include "phys_alloc.h"

ckern::ProcessMemoryCtx::ProcessMemoryCtx()
{
  pml4_phys = memory::phys_alloc.alloc_pages(Paging::PAGE_SIZE);
  pml4 = reinterpret_cast<Paging::PML4Entry *>(Paging::P2V(reinterpret_cast<uintptr_t>(pml4_phys->base)));
  Util::memcpy(pml4, Paging::kern_pml4, sizeof(*pml4) * 512);
}

ckern::ProcessMemoryCtx::~ProcessMemoryCtx()
{
  memory::phys_alloc.free_page(pml4);
}

void ckern::ProcessMemoryCtx::switch_pages()
{
  Paging::switch_pages(&pml4[0]);
}

void ckern::Process::activate()
{
  memory_ctx.switch_pages();
}
