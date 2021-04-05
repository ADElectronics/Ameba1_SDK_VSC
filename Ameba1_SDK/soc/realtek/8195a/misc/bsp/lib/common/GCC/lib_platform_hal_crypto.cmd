@echo off
cls

set libname=hal_crypto

cd lib_platform.lib

arm-none-eabi-nm.exe -S %libname%.o

echo done.
pause > nul