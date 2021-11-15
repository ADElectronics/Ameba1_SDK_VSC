
# ------------------------------------------------------ Compilation tools --------------------------------------------------------

CROSS_COMPILE = $(ARM_GCC_TOOLCHAIN)arm-none-eabi-
AR = $(CROSS_COMPILE)ar.exe
CC = $(CROSS_COMPILE)gcc.exe
AS = $(CROSS_COMPILE)as.exe
NM = $(CROSS_COMPILE)nm.exe
LD = $(CROSS_COMPILE)gcc.exe
GDB = $(CROSS_COMPILE)gdb.exe
OBJCOPY = $(CROSS_COMPILE)objcopy.exe
OBJDUMP = $(CROSS_COMPILE)objdump.exe
SZ = $(CROSS_COMPILE)size.exe

PICK = $(AMEBA_TOOLDIR)pick.exe
PAD = $(AMEBA_TOOLDIR)padding.exe
CHKSUM = $(AMEBA_TOOLDIR)checksum.exe
OTA = $(AMEBA_TOOLDIR)ota.exe

# ------------------------------------------------------ WEB File System tools ---------------------------------------------------

WEBFS = $(AMEBA_TOOLDIR)webfs.exe
