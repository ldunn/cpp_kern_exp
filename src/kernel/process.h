#include "paging.h"
#include "phys_alloc.h"

#ifndef CKERN_PROCESS_H
#define CKERN_PROCESS_H

namespace ckern
{
  class ProcessMemoryCtx
  {
    public:
      ProcessMemoryCtx();

      ~ProcessMemoryCtx();

      void switch_pages();
    private:
      memory::kunique_ptr<memory::PhysAllocation> pml4_phys;
      ckern::Paging::PML4Entry *pml4;
  };

  class Process
  {
    public:

      void activate();

    private:

      ProcessMemoryCtx memory_ctx;
  };


}

#endif
