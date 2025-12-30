#include "elf.h"
#include "util.h"

elf64_addr load_elf(char *start)
{
  elf64_ehdr *hdr = (elf64_ehdr *)start;
  elf64_shdr *shdr_table = (elf64_shdr *)(start + hdr->e_shoff);
  elf64_phdr *phdr_table = (elf64_phdr *)(start + hdr->e_phoff);

  for (int seg = 0; seg < hdr->e_phnum; seg++)
  {
    elf64_phdr *phdr = &phdr_table[seg];

    void *src = start + phdr->p_offset;
    memcpy((char*)phdr->p_paddr, src, phdr->p_filesz);
  }

  return hdr->e_entry;
}
