add_library(loader OBJECT
    loader.s
)

add_executable(kernel.elf
    kernel.cpp
    video.cpp
    util.cpp
    gdt.cpp
    timer.cpp
    keyboard.cpp
    interrupts.cpp
    portio.s
    interrupts.s
    gdt.s
)

set_target_properties(kernel.elf PROPERTIES LINKER_LANGUAGE CXX)

target_link_libraries(kernel.elf PUBLIC loader)

target_link_options(kernel.elf PUBLIC -static LINKER:-fuse-ld=ld -Wl,-T${CMAKE_CURRENT_SOURCE_DIR}/link.ld,-melf_i386,--build-id=none,--no-dynamic-linker,--static)
