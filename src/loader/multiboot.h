#ifndef LOADER_MULTIBOOT_H
#define LOADER_MULTIBOOT_H

typedef struct __attribute__((packed))
{
  void *mod_start;
  void *mod_end;

  char *string;

  unsigned reserved;
} multiboot_mod_t;

typedef struct __attribute__((packed))
{
  unsigned flags;
  unsigned mem_lower;
  unsigned mem_upper;

  unsigned boot_device;

  char *cmd_line;

  unsigned mods_count;
  multiboot_mod_t *mods;

} multiboot_header_t;


#endif
