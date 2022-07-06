/********************************************************
memory map
    Start Address   Size    CM4 view Devices
    0x0000_0000     64MB    Reserved
    0x0400_0000     192KB   CM4 TCM / cache
    0x0403_0000     832KB   Reserved
    0x0410_0000     32KB    CM4 ROM
    0x0410_8000     992KB   Reserved
    0x0420_0000     400K    SYSRAM
    0x0800_0000     128MB   SFC0
    0x1000_0000     128MB   SW virtual memory management
    0x1800_0000     128MB   SW virtual memory management
*********************************************************/

OUTPUT_FORMAT("elf32-littlearm", "elf32-littlearm", "elf32-littlearm")
OUTPUT_ARCH(arm)

/* Memory Spaces Definitions */
MEMORY
{
    ROM_PARTITION_TABLE(rx)        : ORIGIN = 0x08000000, LENGTH = 4K        /* DOWNLOAD, name: partition_table.bin, display: PartitionTable */
    ROM_SECURITY_HEAD1(rx)         : ORIGIN = 0x08001000, LENGTH = 4K
    ROM_SECURITY_HEAD2(rx)         : ORIGIN = 0x08002000, LENGTH = 4K
    ROM_BL(rx)                     : ORIGIN = 0x08003000, LENGTH = 64K       /* DOWNLOAD, name: bootloader.bin, display: BootLoader */
    ROM_RTOS(rx)                   : ORIGIN = 0x08013000, LENGTH = 1256K     /* DOWNLOAD, name: freertos_create_thread.bin, display: MCU_FW */
    ROM_DSP0(rx)                   : ORIGIN = 0x0814D000, LENGTH = 668K      /* DOWNLOAD, name: dsp0_freertos_create_thread.bin, display: DSP_FW */
    ROM_ROFS(rx)                   : ORIGIN = 0x081F4000, LENGTH = 352K      /* DOWNLOAD, name: filesystem.bin, display: ROFS */
    ROM_FOTA_RESERVED(rx)          : ORIGIN = 0x0824C000, LENGTH = 1680K
    ROM_NVDM_RESERVED(rx)          : ORIGIN = 0x083F0000, LENGTH = 64K       /* DOWNLOAD, name: nvdm.bin, display: NVDM */
    TCM (rwx)                      : ORIGIN = 0x04000000, LENGTH = 176K
    SYSRAM (rwx)                   : ORIGIN = 0x04200000, LENGTH = 348K
    VSYSRAM (rwx)                  : ORIGIN = 0x14200000, LENGTH = 348K
    SYSRAM_DSP0 (rwx)              : ORIGIN = 0x04257000, LENGTH = 8K
    VSYSRAM_DSP0 (rwx)             : ORIGIN = 0x14257000, LENGTH = 8K
    SYSRAM_PRIVATE (rwx)           : ORIGIN = 0x04259000, LENGTH = 12K
    VSYSRAM_PRIVATE (rwx)          : ORIGIN = 0x14259000, LENGTH = 12K
    SYSRAM_BT (rwx)                : ORIGIN = 0x0425C000, LENGTH = 32K
    ROM_LOG_STRING(rx)             : ORIGIN = 0x06000000, LENGTH = 32M
}

 /* Highest address of the stack */
_stack_end = ORIGIN(TCM) + LENGTH(TCM);    /* end of TCM */

/* Generate a link error if stack don't fit into TCM */
_stack_size = 0x1000; /* required amount of stack 4KB*/

/* stack start */
_stack_start = _stack_end - _stack_size;

/* Linker symbol provided for C/Assembly Code */
_sysram_start       = ORIGIN(SYSRAM);
_sysram_length      = LENGTH(SYSRAM);
_vsysram_start      = ORIGIN(VSYSRAM);
_vsysram_length     = LENGTH(VSYSRAM);
_sysram_bt_start    = ORIGIN(SYSRAM_BT);
_sysram_bt_length   = LENGTH(SYSRAM_BT);
_tcm_start          = ORIGIN(TCM);
_tcm_length         = LENGTH(TCM);

/* swla start and size */
_swla_size = 0;       /* default swla will use free ram as the working memory, and also can reserve a region for swla by modify the value of _swla_size */
_swla_start = ORIGIN(VSYSRAM) + LENGTH(VSYSRAM) - _swla_size; /* at the tail of VRAM */

/* Entry Point */
ENTRY(Reset_Handler)

SECTIONS
{
    /* log string */
    .log_str :
    {
        KEEP(*(.log_version))
        KEEP(*(.log_timestamp))
        KEEP(*(SORT_BY_NAME(.log_filter_mirror*)))
        KEEP(*(.log_filter_end))
        *(.log_string)
        *(.assert_string*)
    } > ROM_LOG_STRING

    /* Code & rodata region */
    . = ORIGIN(SYSRAM);
    . = ALIGN(4);
    .text :ALIGN(4)
    {
        _text_start = .;
        Image$$TEXT$$Base = .;

        KEEP(*(.reset_handler*))
        KEEP(*(.init))
        KEEP(*(.fini))
        *(.text)
        *(.text*)
        *(.rodata)
        *(.rodata*)

        . = ALIGN(4);
        _log_filter_start = .;
        KEEP(*(SORT_BY_NAME(.log_filter.*)))
        _log_filter_end = .;

        . = ALIGN(4);
        Image$$TEXT$$Limit = .;
        _text_end = .;
    } > SYSRAM  AT> SYSRAM

    /* GNU region for unwinding the stack */
    .ARM.extab :
    {
        *(.ARM.extab* .gnu.linkonce.armextab.*)
    } > SYSRAM  AT> SYSRAM

    /* GNU region for unwinding the stack */
    .ARM.exidx :
    {
        __exidx_start = .;
        *(.ARM.exidx* .gnu.linkonce.armexidx.*)
        __exidx_end = .;
    } > SYSRAM  AT> SYSRAM

    /* Sysram non-cacheable rwdata & shareable rwdata region */
    . = ALIGN(4);
    _ram_noncached_rw_load = LOADADDR(.noncached_data);
    _noncached_sysram_rw_load = LOADADDR(.noncached_data);
    _share_data_load = LOADADDR(.noncached_data);
    .noncached_data :ALIGN(4)
    {
        . = ALIGN(4);
        Image$$NONCACHED_RAM_DATA$$Base = .;
        Image$$NONCACHED_SYSRAM_DATA$$Base = .;
        Image$$SHARE_DATA$$Base = .;
        _ram_noncached_rw_start = .;
        _noncached_sysram_rw_start = .;
        _share_data_start = .;

        *(.noncached_ram_rwdata)
        *(.noncached_ram_rwdata*)
        *(.noncached_sysram_rwdata)
        *(.noncached_sysram_rwdata*)
        *(.share_rwdata)
        *(.share_rwdata*)

        Image$$NONCACHED_RAM_DATA$$Limit = .;
        Image$$NONCACHED_SYSRAM_DATA$$Limit = .;
        Image$$SHARE_DATA$$Limit = .;
        _ram_noncached_rw_end = .;
        _noncached_sysram_rw_end = .;
        _share_data_end = .;
    } > SYSRAM  AT> SYSRAM

    /* Sysram non-cacheable zidata region */
    . = ALIGN(4);
    .noncached_bss :
    {
        Image$$NONCACHED_RAM_ZI$$Base = .;
        Image$$NONCACHED_SYSRAM_ZI$$Base = .;
        Image$$SHARE_ZI$$Base = .;
        _ram_noncached_zi_start = .;
        _noncached_sysram_zi_start = .;
        _share_bss_start = .;

        *(.noncached_ram_zidata)
        *(.noncached_ram_zidata*)
        *(.noncached_sysram_zidata)
        *(.noncached_sysram_zidata*)
        *(.share_zidata)
        *(.share_zidata*)

        Image$$NONCACHED_RAM_ZI$$Limit = .;
        Image$$NONCACHED_SYSRAM_ZI$$Limit = .;
        Image$$SHARE_ZI$$Limit = .;
        _ram_noncached_zi_end = .;
        _noncached_sysram_zi_end = .;
        _share_bss_end = .;
    } > SYSRAM  AT> SYSRAM

    /* Sysram code & rodata region */
    . = . + ORIGIN(VSYSRAM) - ORIGIN(SYSRAM);
    . = ALIGN(32);
    _ram_code_load = LOADADDR(.cached_ram_text);
    _sysram_code_load = LOADADDR(.cached_ram_text);
    .cached_ram_text . :ALIGN(32)
    {
        Image$$CACHED_RAM_TEXT$$Base = .;
        Image$$CACHED_SYSRAM_TEXT$$Base = .;
        _ram_code_start = .;
        _sysram_code_start = .;

        *(.ram_code)
        *(.ram_code*)
        *(.sysram_code)
        *(.sysram_code*)
        *(.ram_rodata)
        *(.ram_rodata*)
        *(.sysram_rodata)
        *(.sysram_rodata*)

        Image$$CACHED_RAM_TEXT$$Limit = .;
        Image$$CACHED_SYSRAM_TEXT$$Limit = .;
        _ram_code_end = .;
        _sysram_code_end = .;
    } > VSYSRAM  AT> SYSRAM

    /* Sysram cacheable rwdata region */
    . = ALIGN(32);
    _data_load = LOADADDR(.cached_ram_data);
    _cached_sysram_data_load = LOADADDR(.cached_ram_data);
    .cached_ram_data . :ALIGN(32)
    {
        _data_start = .;
        _cached_sysram_data_start = .;
        Image$$CACHED_RAM_DATA$$RW$$Base = .;
        Image$$CACHED_SYSRAM_DATA$$RW$$Base = .;

        *(.data)
        *(.data*)
        *(.cached_sysram_rwdata)
        *(.cached_sysram_rwdata*)

        . = ALIGN(4);
        /* preinit data */
        PROVIDE (__preinit_array_start = .);
        KEEP(*(.preinit_array))
        PROVIDE (__preinit_array_end = .);

        . = ALIGN(4);
        /* init data */
        PROVIDE (__init_array_start = .);
        KEEP(*(SORT(.init_array.*)))
        KEEP(*(.init_array))
        PROVIDE (__init_array_end = .);

        . = ALIGN(4);
        /* finit data */
        PROVIDE (__fini_array_start = .);
        KEEP(*(SORT(.fini_array.*)))
        KEEP(*(.fini_array))
        PROVIDE (__fini_array_end = .);

        _data_end = .;
        _cached_sysram_data_end = .;
        Image$$CACHED_RAM_DATA$$RW$$Limit = .;
        Image$$CACHED_SYSRAM_DATA$$RW$$Limit = .;
    } > VSYSRAM  AT> SYSRAM

    /* Sysram cacheable zidata region */
    . = ALIGN(4);
    .bss. :
    {
        _bss_start = .;
        _cached_sysram_bss_start = .;
        Image$$CACHED_RAM_DATA$$ZI$$Base = .;
        Image$$CACHED_SYSRAM_DATA$$ZI$$Base = .;

        *(.bss)
        *(.bss*)
        *(COMMON)
        *(.cached_sysram_zidata)
        *(.cached_sysram_zidata*)

        . = ALIGN(4);
        _bss_end = .;
        _cached_sysram_bss_end = .;
        Image$$CACHED_RAM_DATA$$ZI$$Limit = .;
        Image$$CACHED_SYSRAM_DATA$$ZI$$Limit = .;
    } > VSYSRAM  AT> SYSRAM

    ASSERT(_bss_end <= (ORIGIN(VSYSRAM) + LENGTH(VSYSRAM)),"SYSRAM expired")

    /* bt shareable rwdata region */
    . = ALIGN(4);
    _bt_sysram_rw_data_load = LOADADDR(.bt_sysram_rw_data);
    .bt_sysram_rw_data :ALIGN(4)
    {
        _bt_sysram_rw_data_start = .;
        Image$$BT_SYSRAM_RW_DATA$$RW$$Base = .;

        *(.share_rwbt)
        *(.share_rwbt*)

        . = ALIGN(4);
        Image$$BT_SYSRAM_RW_DATA$$RW$$Limit = .;
        _bt_sysram_rw_data_end = .;
    }> SYSRAM_BT  AT> SYSRAM

    /* bt shareable zidata region */
    . = ALIGN(4);
    _bt_sysram_zi_data_load = LOADADDR(.bt_sysram_zi_data);
    .bt_sysram_zi_data (NOLOAD) :ALIGN(4)
    {
        _bt_sysram_zi_data_start = .;
        Image$$BT_SYSRAM_ZI_DATA$$RW$$Base = .;

        *(.share_zibt)
        *(.share_zibt*)

        . = ALIGN(4);
        Image$$BT_SYSRAM_ZI_DATA$$RW$$Limit = .;
        _bt_sysram_zi_data_end = .;
    }> SYSRAM_BT  AT> SYSRAM

    /* TCM code & rodata & rwdata region */
    . = ALIGN(4);
    _tcm_text_load = LOADADDR(.tcm);
    .tcm :ALIGN(4)
    {
        _tcm_text_start = .;
        Image$$TCM$$RO$$Base = .;
        Image$$VECTOR$$TABLE$$Base = .;

        KEEP(*(.isr_vector))
        *(.exception_code)
        *(.exception_code*)
        *(.exception_rodata)
        *(.exception_rodata*)
        *(.tcm_code)
        *(.tcm_code*)
        *(.tcm_rodata)
        *(.tcm_rodata*)

        Image$$TCM$$RO$$Limit = .;
        Image$$TCM$$RW$$Base = .;

        *(.tcm_rwdata)
        *(.tcm_rwdata*)

        . = ALIGN(4);
        Image$$TCM$$RW$$Limit = .;
        _tcm_text_end = .;
    }> TCM  AT> SYSRAM

    /* TCM zidata region */
    . = ALIGN(4);
    .tcm_bss (NOLOAD) :
    {
        _tcm_zi_start = .;
        Image$$TCM$$ZI$$Base = .;

        *(.tcm_zidata)
        *(.tcm_zidata*)

        _tcm_zi_end = .;
        Image$$TCM$$ZI$$Limit = .;
    }> TCM  AT> SYSRAM

    /* use to check if the stack exceeds the total TCM size*/
    .stack :
    {
        . = ALIGN(4);
        PROVIDE ( end = . );
        PROVIDE ( _end = . );
        . = . + _stack_size;
        . = ALIGN(4);
    } > TCM

    Image$$STACK$$ZI$$Base = _stack_end - _stack_size;
    Image$$STACK$$ZI$$Limit = _stack_end;

    /* provide nvdm start and length for NVDM management */
    Image$$NVDM$$ZI$$Base = ORIGIN(ROM_NVDM_RESERVED);
    Image$$NVDM$$ZI$$Length = LENGTH(ROM_NVDM_RESERVED);
    Image$$NVDM$$ZI$$Limit = ORIGIN(ROM_NVDM_RESERVED) + LENGTH(ROM_NVDM_RESERVED);
}
