/*
 * GENERATED FILE - DO NOT EDIT
 * Copyright (c) 2008-2013 Code Red Technologies Ltd,
 * Copyright 2015, 2018-2019 NXP
 * (c) NXP Semiconductors 2013-2022
 * Generated linker script file for LPC1769
 * Created from memory.ldt by FMCreateLinkMemory
 * Using Freemarker v2.3.30
 * MCUXpresso IDE v11.6.0 [Build 8187] [2022-07-13] on 29 oct. 2022 1:02:40
 */

MEMORY
{
  /* Define each memory region */
  MFlash512 (rx) : ORIGIN = 0x0, LENGTH = 0x80000 /* 512K bytes (alias Flash) */  
  RamLoc32 (rwx) : ORIGIN = 0x10000000, LENGTH = 0x8000 /* 32K bytes (alias RAM) */  
  RamAHB32 (rwx) : ORIGIN = 0x2007c000, LENGTH = 0x8000 /* 32K bytes (alias RAM2) */  
}

  /* Define a symbol for the top of each memory region */
  __base_MFlash512 = 0x0  ; /* MFlash512 */  
  __base_Flash = 0x0 ; /* Flash */  
  __top_MFlash512 = 0x0 + 0x80000 ; /* 512K bytes */  
  __top_Flash = 0x0 + 0x80000 ; /* 512K bytes */  
  __base_RamLoc32 = 0x10000000  ; /* RamLoc32 */  
  __base_RAM = 0x10000000 ; /* RAM */  
  __top_RamLoc32 = 0x10000000 + 0x8000 ; /* 32K bytes */  
  __top_RAM = 0x10000000 + 0x8000 ; /* 32K bytes */  
  __base_RamAHB32 = 0x2007c000  ; /* RamAHB32 */  
  __base_RAM2 = 0x2007c000 ; /* RAM2 */  
  __top_RamAHB32 = 0x2007c000 + 0x8000 ; /* 32K bytes */  
  __top_RAM2 = 0x2007c000 + 0x8000 ; /* 32K bytes */  
