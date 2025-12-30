#ifndef LOADER_ELF_H
#define LOADER_ELF_H

typedef unsigned long long elf64_addr;
typedef unsigned long long elf64_off;
typedef unsigned short elf64_half;
typedef unsigned int elf64_word;
typedef int elf64_sword;
typedef unsigned long long elf64_xword;
typedef long long elf64_sxword;

typedef struct __attribute__((packed))
{
  unsigned char   e_ident[16];
  elf64_half      e_type;
  elf64_half      e_machine;
  elf64_word      e_version;
  elf64_addr      e_entry;
  elf64_off       e_phoff;
  elf64_off       e_shoff;
  elf64_word      e_flags;
  elf64_half      e_ehsize;
  elf64_half      e_phentsize;
  elf64_half      e_phnum;
  elf64_half      e_shentsize;
  elf64_half      e_shnum;
  elf64_half      e_shstrndx;
} elf64_ehdr;

typedef struct __attribute__((packed))
{
  elf64_word      sh_name;
  elf64_word      sh_type;
  elf64_xword     sh_flags;
  elf64_addr      sh_asddr;
  elf64_off       sh_offset;
  elf64_xword     sh_size;
  elf64_word      sh_link;
  elf64_xword     sh_addralign;
  elf64_xword     sh_entsize;
} elf64_shdr;

typedef struct __attribute__((packed))
{
  elf64_word      p_type;
  elf64_word      p_flags;
  elf64_off       p_offset;
  elf64_addr      p_vaddr;
  elf64_addr      p_paddr;
  elf64_xword     p_filesz;
  elf64_xword     p_memsz;
  elf64_xword     p_align;
} elf64_phdr;

#define SHF_WRITE 0x1
#define SHF_ALLOC 0x2
#define SHF_EXECINSTR 0x4

elf64_addr load_elf(char *start);

#endif
