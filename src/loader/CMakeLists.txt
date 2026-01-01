add_executable(loader
    loader.s
    loader.c
    elf.c
    paging.c
    util.c
)
set_target_properties(loader PROPERTIES NASM_OBJ_FORMAT elf32)
set_target_properties(loader PROPERTIES LINKER_LANGUAGE C)
target_link_options(loader PUBLIC -static LINKER:-fuse-ld=ld -Wl,-T${CMAKE_CURRENT_SOURCE_DIR}/loader_link.ld,-melf_i386,--build-id=none,--no-dynamic-linker,--static)
