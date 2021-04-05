

# ------------------------------------------------------ System settings --------------------------------------------------------

# Компилировать оригинальные библиотеки из SDK ?
APP_POLAR_SSL = 0
APP_MQTT = 0
APP_HTTPC = 0
APP_HTTPD = 0
APP_MDNS = 0
APP_WSCLIENT = 0

# Компилировать оригинальные примеры из SDK ?
APP_SDK_EXAMPLES = 1

# Компилировать SDIO библиотеку pvvx ?
USE_SDIOH = 0

# Использовать оригинальный startup от Ameba ? (иначе используется startup файл от pvvx)
USE_AMEBA_STARTUP = 0

# 8.1.2 (оригинал) \ 9.0.0 (поновее чуток)
FREERTOS_VERSION = 9.0.0
# 1.4.1 (оригинал) \ 2.1.2 (свежая версия с плюшками)
LWIP_VERSION = 2.1.2
# Компилировать плюшки (apps) из 2.1.2 LwIP ?
LWIP_212_APPS = 1

# ------------------------------------------------------ Compilation tools ------------------------------------------------------

ARM_GCC_TOOLCHAIN =
AMEBA1_SDK_PATH = ../AMEBA1_SDK/
AMEBA_TOOLDIR = $(AMEBA1_SDK_PATH)soc/realtek/8195a/misc/iar_utility/common/tools/
FLASH_TOOLDIR = $(AMEBA1_SDK_PATH)soc/realtek/8195a/misc/gcc_utility/
PYTHON = python
JLINK_TOOLS_PATH = JLink/

# ------------------------------------------------------ Debug tools ------------------------------------------------------------

JLINK_PATH =
JLINK_GDB = JLinkGDBServer.exe
JLINK_CONSOLE = JLink.exe
JLINK_SPEED = 12000
JLINK_INTERFACE = JTAG
