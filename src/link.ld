KERNEL_OFFSET = 0;
KERNEL_START = 0x20000;

ENTRY(kmain)

SECTIONS {
    . = KERNEL_START + KERNEL_OFFSET;

    kernel_start = .;

    .text ALIGN (4K): AT(ADDR(.text) - KERNEL_OFFSET)
    {
        *(.text)
    }

    .rodata ALIGN (4K): AT(ADDR(.rodata) - KERNEL_OFFSET)
    {
        *(.rodata*)
    }

    .data ALIGN (4K): AT(ADDR(.data) - KERNEL_OFFSET)
    {
        *(.data)
    }

    .bss ALIGN (4K): AT(ADDR(.bss) - KERNEL_OFFSET)
    {
        *(COMMON)
        *(.bss)
    }

    kernel_end =.;
}
