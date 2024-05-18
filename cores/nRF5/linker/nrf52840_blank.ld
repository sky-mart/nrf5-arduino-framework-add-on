/* Linker script to configure memory regions. */

SEARCH_DIR(.)
GROUP(-lgcc -lc -lnosys)

MEMORY
{

  /* FLASH memory structure (with MBR and Nordic's DFU bootloader):
   *  0x00 - 0x0FFF - MBR region
   *  0x1000 - 0x0DFFFF - Application region
   *  0x0E0000 - 0x0FDFFF - Bootloader region
   */
  FLASH (rx)     : ORIGIN = 0x1000, LENGTH = 0xE0000 - 0x1000

  /* 
   * Application only uses MBR, that's why RAM start address is 0x20000008.
   * For more info see https://devzone.nordicsemi.com/guides/short-range-guides/b/getting-started/posts/adjustment-of-ram-and-flash-memory
   */ 
  RAM (rwx) :  ORIGIN = 0x20000008, LENGTH = 0x20040000 - 0x20000008
}

SECTIONS
{
  . = ALIGN(4);
  .svc_data :
  {
    PROVIDE(__start_svc_data = .);
    KEEP(*(.svc_data))
    PROVIDE(__stop_svc_data = .);
  } > RAM
  
  .fs_data :
  {
    PROVIDE(__start_fs_data = .);
    KEEP(*(.fs_data))
    PROVIDE(__stop_fs_data = .);
  } > RAM
} INSERT AFTER .data;

INCLUDE "nrf52_common.ld"
