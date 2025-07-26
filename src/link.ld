OUTPUT_FORMAT(elf32-i386)
ENTRY(start)
SECTIONS
{
    . = 0x100000;
    .text : ALIGN(4K) { *(.text) } :text =0
    .rodata : ALIGN(4K) { *(.rodata) } :rodata
    .data : ALIGN(4K) { *(.data) } :data
    .bss : ALIGN(4K) { *(.bss) } :bss
}
PHDRS
{
    text PT_LOAD FLAGS(5);
    rodata PT_LOAD FLAGS(4);
    data PT_LOAD FLAGS(6);
    bss PT_LOAD FLAGS(6);
}