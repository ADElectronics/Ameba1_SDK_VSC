# ------------------------------------------------------ System settings --------------------------------------------------------

# Номер образа (1 или 2)
OTA_INDEX = 1

# Компилировать оригинальные библиотеки из SDK ?
APP_POLAR_SSL = 0
APP_MQTT = 0
APP_HTTPC = 0
APP_HTTPD = 0
APP_MDNS = 0
APP_WSCLIENT = 0
# Адаптированный WEB сервер от pvvx
APP_WEBSERVER = 1

# Компилировать оригинальные примеры из SDK ?
APP_SDK_EXAMPLES = 1

# 8.1.2 (оригинал) \ 9.0.0 (поновее чуток, но ещё не допилил)
FREERTOS_VERSION = 8.1.2

# 1.4.1 (оригинал) \ 2.1.2 (свежая версия с плюшками)
LWIP_VERSION = 2.1.2

# Компилировать плюшки (apps) из 2.1.2 LwIP ?
LWIP_212_APPS = 0

# ------------------------------------------------------ Compilation tools ------------------------------------------------------

ARM_GCC_TOOLCHAIN =
AMEBA1_SDK_PATH = ../AMEBA1_SDK/
AMEBA_TOOLDIR = $(AMEBA1_SDK_PATH)soc/realtek/8711b/misc/iar_utility/common/tools/
FLASH_TOOLDIR = $(AMEBA1_SDK_PATH)soc/realtek/8195a/misc/gcc_utility/
FLASHDOWNLOAD_TOOLDIR = $(AMEBA1_SDK_PATH)soc/realtek/8711b/misc/gnu_utility/flash_download/image/
JLINK_TOOLS_PATH = JLink/

# ------------------------------------------------------ Debug tools ------------------------------------------------------------

JLINK_PATH =
JLINK_GDB = JLinkGDBServer.exe
JLINK_CONSOLE = JLink.exe
JLINK_SPEED = 12000
JLINK_INTERFACE = SWD
