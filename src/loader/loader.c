#include "multiboot.h"
#include "elf.h"

//#define KERNEL_OFFSET 0xFFFFFF8000000000
#define KERNEL_OFFSET 0x0

int loader_main(int mb_magic, multiboot_header_t *ptr)
{
  char *kernel_elf_start = ptr->mods[0].mod_start;

  elf64_addr res = load_elf(kernel_elf_start);

  return ((int)res - KERNEL_OFFSET);
}
