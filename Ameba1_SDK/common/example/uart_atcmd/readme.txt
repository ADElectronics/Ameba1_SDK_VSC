uart atcmd example is for AT command v2
For AT command v2, please refer to "AN0075 Realtek Ameba-all at command v2.0".pdf

For Ameba 1, if cannot build pass due to large code size in SRAM, you could
1. link some codes in SDRAM, 
2. or reduce SRAM heap (configTOTAL_HEAP_SIZE ) and enable SDRAM heap in heap_5.c.