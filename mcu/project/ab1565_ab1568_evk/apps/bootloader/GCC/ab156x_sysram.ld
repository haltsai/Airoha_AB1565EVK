/********************************************************
memory map
    Start Address   Size    CM4 view Devices
    0x0000_0000     64MB    Reserved
    0x0400_0000     192KB   CM4 TCM / cache
    0x0403_0000     832KB   Reserved
    0x0410_0000     32KB    CM4 ROM
    0x0410_8000     992KB   Reserved
    0x0420_0000     384K    SYSRAM
    0x0800_0000     128MB   SFC0
    0x1000_0000     128MB   SW virtual memory management
    0x1800_0000     128MB   SW virtual memory management
*********************************************************/

OUTPUT_FORMAT("elf32-littlearm", "elf32-littlearm", "elf32-littlearm")
OUTPUT_ARCH(arm)

/* Generate a link error if heap and stack don't fit into RAM */
_Min_Heap_Size = 0;      /* required amount of heap  */
_Min_Stack_Size = 0x400; /* required amount of stack */

/* Memory Spaces Definitions:4M FLASH,4M PSRAM,384k SYSRAM */
MEMORY
{
    TCM (rwx)             : ORIGIN = 0x04000000, LENGTH = 64K
    TCM_STACK (rwx)       : ORIGIN = 0x04010000, LENGTH = 20K
    TCM_BSS (rwx)         : ORIGIN = 0x04015000, LENGTH = 64K
    SYSRAM (rwx)          : ORIGIN = 0x04200000, LENGTH = 128K
    SYSRAM_NC (rwx)       : ORIGIN = 0x04220000, LENGTH = 128K
    ROM_BL(rx)            : ORIGIN = 0x08003000, LENGTH = 64K        /* DOWNLOAD, name: bootloader.bin, display: BootLoader */
}

 /* Highest address of the stack */
_estack = ORIGIN(TCM_STACK) + LENGTH(TCM_STACK);    /* end of TCM */

/* Generate a link error if stack don't fit into TCM */
_stack_size = 0x1000; /* required amount of stack 4KB*/

/* stack start */
_stack_start = _estack - _stack_size;

/* Entry Point */
ENTRY(Reset_Handler)

/* Define output sections */
SECTIONS
{
    . = ORIGIN(ROM_BL);
  /* The startup code goes first into FLASH */
  .isr_vector :
  {
    . = ALIGN(4);
    KEEP(*(.reset_handler))
    KEEP(*(.isr_vector))
    KEEP(*(.rom_parameter))
    . = ALIGN(4);
  } > TCM AT > ROM_BL

  /* The program code and other data goes into FLASH */
  .text :
  {
    . = ALIGN(4);
    *(.text)           /* .text sections (code) */
    *(.text*)          /* .text* sections (code) */
    *(.rodata)         /* .rodata sections (constants, strings, etc.) */
    *(.rodata*)        /* .rodata* sections (constants, strings, etc.) */
    *(.ram_rodata*)
    *(.sysram_rodata*)
    *(.tcm_rodata*)
    *(.glue_7)         /* glue arm to thumb code */
    *(.glue_7t)        /* glue thumb to arm code */
    *(.eh_frame)
    *(.ram_code*)
    *(.sysram_code*)
    *(.tcm_code*)

    KEEP (*(.init))
    KEEP (*(.fini))

    . = ALIGN(4);
    _etext = .;        /* define a global symbols at end of code */
    _exit = .;
  } >TCM AT > ROM_BL


  /* used by the startup to initialize data */
  _sidata = LOADADDR(.data);

  /* Initialized data sections goes into RAM, load LMA copy after code */
  .data :
  {
    . = ALIGN(4);
    _sdata = .;        /* create a global symbol at data start */
    *(.data)           /* .data sections */
    *(.data*)          /* .data* sections */
    *(.tcm_rwdata*)
    *(.cached_sysram_rwdata*)
		*(.noncached_sysram_rwdata*)
		*(.noncached_ram_rwdata*)
    . = ALIGN(4);
    _edata = .;        /* define a global symbol at data end */
  } >TCM AT > ROM_BL


  . = ALIGN(4);
  .sysram_bss (NOLOAD) :
  {
    /* This is used by the startup in order to initialize the .bss secion */
    _sysram_sbss = .;         /* define a global symbol at bss start */
    *(.cached_sysram_zidata_sboot)
    . = ALIGN(4);
    _sysram_ebss = .;         /* define a global symbol at bss end */
  } >SYSRAM AT > ROM_BL

   /* Uninitialized data section */
  . = ALIGN(4);
  .bss (NOLOAD) :
  {
    /* This is used by the startup in order to initialize the .bss secion */
    _sbss = .;         /* define a global symbol at bss start */
    __bss_start__ = _sbss;
    *(.bss)
    *(.bss*)
    *(COMMON)
    *(.tcm_zidata*)
    *(.cached_sysram_zidata*)
    *(.noncached_sysram_zidata*)
    *(.noncached_ram_zidata*)
    . = ALIGN(4);
    __end__ = .;
    _ebss = .;         /* define a global symbol at bss end */
    __bss_end__ = _ebss;
  } >TCM_BSS AT > ROM_BL

  /* User_heap_stack section, used to check that there is enough RAM left */
  ._user_heap_stack :
  {
    . = ALIGN(4);
    PROVIDE ( end = . );
    PROVIDE ( _end = . );
    __HeapBase = .;
    . = . + _Min_Heap_Size;
    __HeapLimit = .;
    __StackLimit = .;
    . = . + _Min_Stack_Size;
    . = ALIGN(4);
  } >TCM AT > ROM_BL
}
