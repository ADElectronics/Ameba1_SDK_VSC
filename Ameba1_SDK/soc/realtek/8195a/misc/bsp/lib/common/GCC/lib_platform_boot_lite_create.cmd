@echo off
cls

set libname=lib_platform

if exist %libname%_boot_lite.a del %libname%_boot_lite.a
if exist %libname%.lib/*.* rd /q /s %libname%.lib

md %libname%.lib
cd %libname%.lib

arm-none-eabi-ar.exe x ..\%libname%.a
del hal_efuse.o
del hal_common.o
del freertos_pmu_8195a.o
del hal_soc_ps_monitor.o
rem del app_start.o
del hal_log_uart.o
del hal_pinmux.o
del hal_misc.o
rem del startup.o
rem del hal_spi_flash_ram.o

arm-none-eabi-ar.exe ru ..\%libname%_boot_lite.a *.o
cd ..
rem rd /q /s %libname%.lib

echo done.
pause > nul