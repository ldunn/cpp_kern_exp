#include "interrupts.h"
#include "video.h"
#include "gdt.h"
#include "interrupts.h"
#include "timer.h"
#include "keyboard.h"
#include "multiboot.h"
#include "strutil.h"
#include "phys_alloc.h"
#include "paging.h"
#include "heap.h"

extern "C" void enter_ring3();

extern "C" int kmain(uint32_t mb_magic, ckern::MultibootInfo *mb_info)
{
  ckern::Interrupts::disable();
  ckern::Framebuffer::cls();
  ckern::Framebuffer::puts("Hello World\n");
  ckern::Framebuffer::printf("Got multiboot magic: 0x%x\n", mb_magic);
  if (mb_info->have_mmap())
  {
    ckern::Framebuffer::printf("Multiboot memory map - length=%d\n", mb_info->mmap_length);
    const auto mmap_buf = reinterpret_cast<char *>(mb_info->get_mmap());
    auto mmap_ent = mb_info->get_mmap();
    while (reinterpret_cast<char *>(mmap_ent) - mmap_buf < mb_info->mmap_length)
    {
      ckern::Framebuffer::printf("Base: 0x%x Length: 0x%x Type: 0x%x\n", mmap_ent->base, mmap_ent->length, static_cast<int>(mmap_ent->type));
      mmap_ent = mmap_ent->next();
    }
  }
  ckern::GDT::GDT::init();
  ckern::Interrupts::IDT::init();
  ckern::Interrupts::PIC::init();
  ckern::devices::Timer::install();
  ckern::devices::Keyboard::install();

  ckern::memory::init_phys_alloc();

  ckern::memory::phys_alloc.fill_from_multiboot(mb_info);

  // Remove identity mapping - just map 128MiB at -2GB
  ckern::Paging::init_kern_paging();
  uintptr_t phys_addr = 0;
  uintptr_t virt_addr_hi = ckern::Util::KERN_OFFSET;

  while (phys_addr < 128 * 1024 * 1024)
  {
    //ckern::Paging::map_page(phys_addr, virt_addr_lo);
    ckern::Paging::map_page(phys_addr, virt_addr_hi);
    phys_addr += ckern::Paging::PAGE_SIZE;
    virt_addr_hi += ckern::Paging::PAGE_SIZE;
  }

  // Make a new kernel stack just below the -2GB mark TODO make this larger than one page, copy the stack properly
  auto new_stack_phys = reinterpret_cast<char *>(ckern::memory::phys_alloc.alloc_page());
  ckern::Paging::map_page(reinterpret_cast<uintptr_t>(new_stack_phys), ckern::Util::KERN_OFFSET - ckern::Paging::PAGE_SIZE);
  auto new_stack_virt = reinterpret_cast<char *>(new_stack_phys + ckern::Util::KERN_OFFSET);
  char *rsp, *rbp;
  asm volatile("mov %%rsp, %0" : "=r"(rsp));
  asm volatile("mov %%rbp, %0" : "=r"(rbp));
  ptrdiff_t rbp_diff = rbp - rsp;
  ckern::Util::memcpy(new_stack_phys + ckern::Paging::PAGE_SIZE - 128, rsp, 128);
  rsp = new_stack_virt + ckern::Paging::PAGE_SIZE - 128;
  asm volatile("mov %0, %%rsp" ::"r"(rsp));
  asm volatile("mov %0, %%rbp" ::"r"(rsp + rbp_diff));
  
  ckern::Paging::switch_pages(&ckern::Paging::kern_pml4[0]);

  ckern::memory::init_kern_heap();

  auto p = ckern::memory::phys_alloc.alloc_page();
  ckern::memory::phys_alloc.free_page(p);
  auto p2 = ckern::memory::phys_alloc.alloc_page();
  ckern::Framebuffer::printf("Allocated page at 0x%x\n", reinterpret_cast<uintptr_t>(p));
  ckern::Framebuffer::printf("Allocated page at 0x%x\n", reinterpret_cast<uintptr_t>(p2));
  
  auto bar = ckern::memory::init_kern_heap().alloc(1024);
  auto foo = ckern::memory::init_kern_heap().alloc(4096);
  ckern::memory::init_kern_heap().free(foo);
  ckern::memory::init_kern_heap().alloc(421);
  ckern::memory::init_kern_heap().free(bar);
  ckern::memory::init_kern_heap().alloc(34);
  ckern::memory::init_kern_heap().alloc(14);
  ckern::memory::init_kern_heap().alloc(16384);

  ckern::memory::init_kern_heap().dump();

  ckern::Interrupts::enable();

  enter_ring3();

  while (1) {};

  return -1;
}
