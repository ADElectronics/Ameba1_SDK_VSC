@echo off
cls

set libname=lib_platform

if exist %libname%.lib/*.* rd /q /s %libname%.lib

md %libname%.lib
cd %libname%.lib

arm-none-eabi-ar.exe x ..\%libname%.a

echo done.
pause > nul