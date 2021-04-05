@echo off
cls

set filename=rtl8710_flasher

arm-none-eabi-objcopy.exe -I binary -O ihex %filename%.bin %filename%.hex

pause>nul