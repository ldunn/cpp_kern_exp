#include "multiboot.h"
#include "elf.h"
#include "paging.h"

elf64_addr loader_main(int mb_magic, multiboot_header_t *ptr)
{
  char *kernel_elf_start = ptr->mods[0].mod_start;

  elf64_addr res = load_elf(kernel_elf_start);

  init_pt();

  return res;
}
