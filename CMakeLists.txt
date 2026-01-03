cmake_minimum_required(VERSION 3.28.3)

project(CKern VERSION 0.0.1)

set(CMAKE_ASM_NASM_OBJECT_FORMAT elf)
set(CMAKE_LINKER_TYPE BFD)

set(CMAKE_CXX_STANDARD 23)

set(CMAKE_C_FLAGS "-m32 -nostdlib -nostdinc -fno-builtin -fno-stack-protector -nostartfiles -nodefaultlibs")
set(CMAKE_CXX_FLAGS "-ffreestanding -Wall -Wextra -fno-exceptions -fno-rtti -fno-builtin -fno-stack-protector -fno-threadsafe-statics -fmacro-prefix-map=${CMAKE_CURRENT_SOURCE_DIR}=. -nostartfiles -nodefaultlibs -fpic -mcmodel=kernel -mgeneral-regs-only")
set(CMAKE_ASM_NASM_FLAGS "-g")
set(CMAKE_ASM_NASM_OBJECT_FORMAT elf64)

enable_language(ASM_NASM)
enable_language(C)
enable_language(CXX)


set(CMAKE_ASM_NASM_COMPILE_OBJECT "<CMAKE_ASM_NASM_COMPILER> <INCLUDES> <FLAGS> -o <OBJECT> <SOURCE>")

# Create a compile option that operates on ASM_NASM files
# If the target has a property NASM_OBJ_FORMAT, use it, otherwise
# use the environment variable CMAKE_ASM_NASM_OBJECT_FORMAT
add_compile_options(
    "$<$<COMPILE_LANGUAGE:ASM_NASM>:-f $<IF:$<BOOL:$<TARGET_PROPERTY:NASM_OBJ_FORMAT>>, \
    $<TARGET_PROPERTY:NASM_OBJ_FORMAT>, ${CMAKE_ASM_NASM_OBJECT_FORMAT}>>"
)

add_subdirectory(src)

add_custom_target(iso
    ALL
    COMMAND mkdir -p iso/boot/grub
    COMMAND cp ${CMAKE_CURRENT_SOURCE_DIR}/resources/stage2_eltorito iso/boot/grub
    COMMAND cp ${CMAKE_CURRENT_SOURCE_DIR}/resources/menu.lst iso/boot/grub
    COMMAND cp $<TARGET_FILE:loader> iso/boot
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

add_custom_target(qemu-gdb
    ALL
    COMMAND qemu-system-x86_64 -D qemu.log -d int -machine pc,accel=tcg -no-reboot -no-shutdown -cdrom os.iso -s -S -daemonize
    COMMAND gdb -ex "target remote localhost:1234")
set_target_properties(qemu-gdb PROPERTIES EXCLUDE_FROM_ALL TRUE)

add_custom_target(bochs
    ALL
    COMMAND bochs -f ../resources/bochsrc -q
)
set_target_properties(bochs PROPERTIES EXCLUDE_FROM_ALL TRUE)
