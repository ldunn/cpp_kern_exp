KERNEL_OFFSET = 0xffffffff80000000;
KERNEL_START = 16M;

ENTRY(kmain)

SECTIONS {
    . = KERNEL_START + KERNEL_OFFSET;

    _kernel_start = .;

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

    _kernel_end =.;
}
