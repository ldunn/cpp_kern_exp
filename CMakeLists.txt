cmake_minimum_required(VERSION 3.28.3)

project(CKern VERSION 0.0.1)

set(CMAKE_ASM_NASM_OBJECT_FORMAT elf)
set(CMAKE_LINKER_TYPE BFD)

set(CMAKE_CXX_STANDARD 23)

set(CMAKE_C_FLAGS "-m32 -nostdlib -nostdinc -fno-builtin -fno-stack-protector -nostartfiles -nodefaultlibs")
set(CMAKE_CXX_FLAGS "-m32 -ffreestanding -Wall -Wextra -fno-exceptions -fno-rtti -fno-builtin -fno-stack-protector -nostartfiles -nodefaultlibs")
set(CMAKE_ASM_NASM_FLAGS "-g")

enable_language(ASM_NASM)
enable_language(C)
enable_language(CXX)

add_subdirectory(src)

add_custom_target(iso
    ALL
    COMMAND mkdir -p iso/boot/grub
    COMMAND cp ${CMAKE_CURRENT_SOURCE_DIR}/resources/stage2_eltorito iso/boot/grub
    COMMAND cp ${CMAKE_CURRENT_SOURCE_DIR}/resources/menu.lst iso/boot/grub
    COMMAND cp $<TARGET_FILE:kernel.elf> iso/boot
    COMMAND genisoimage -R 
            -b boot/grub/stage2_eltorito 
            -no-emul-boot 
            -boot-load-size 4 
            -A os 
            -input-charset utf8 
            -quiet 
            -boot-info-table 
            -o os.iso 
            iso
    DEPENDS
        kernel.elf
    SOURCES
        resources/menu.lst
)

add_custom_target(qemu
    ALL
    COMMAND qemu-system-i386 -cdrom os.iso
    DEPENDS iso
)
set_target_properties(qemu PROPERTIES EXCLUDE_FROM_ALL TRUE)

add_custom_target(qemu-gdb
    ALL
    COMMAND qemu-system-i386 -cdrom os.iso -s -S -daemonize
    COMMAND gdb -ex "target remote localhost:1234" -ex "file src/kernel.elf")
set_target_properties(qemu-gdb PROPERTIES EXCLUDE_FROM_ALL TRUE)

add_custom_target(bochs
    ALL
    COMMAND bochs -f ../resources/bochsrc -q
)
set_target_properties(bochs PROPERTIES EXCLUDE_FROM_ALL TRUE)
