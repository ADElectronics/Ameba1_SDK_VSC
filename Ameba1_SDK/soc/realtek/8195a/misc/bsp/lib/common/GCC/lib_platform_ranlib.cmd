@echo off
cls

set libname=lib_platform

arm-none-eabi-ranlib.exe %libname%.a

echo done.
pause > nul