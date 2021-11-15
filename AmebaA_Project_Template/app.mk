include app_settings.mk
include $(AMEBA1_SDK_PATH)sdk_tools.mk
include $(AMEBA1_SDK_PATH)sdk_sources_a_main.mk
include $(AMEBA1_SDK_PATH)sdk_sources_a_app.mk

# ---------------------------------------------------- Include folder list ----------------------------------------------------

INCLUDES_USER =
INCLUDES_USER += Src
INCLUDES_USER += Inc

# ---------------------------------------------------- Source file list ----------------------------------------------------

SRC_USER_C =
SRC_USER_C += Src/main.c

# ---------------------------------------------------- Compile options ----------------------------------------------------

LDFILE = rlx8195A-symbol-v04-img2.ld
OPT = -Os

CFLAGS =
CFLAGS += -DM3 -DGCC_ARMCM3 -DCONFIG_PLATFORM_8195A -DF_CPU=166666666L
CFLAGS += -DLWIP_TIMEVAL_PRIVATE=0 -DLWIP_NO_STDINT_H=1 -DLWIP_NO_CTYPE_H=1 -DIN_ADDR_T_DEFINED=1 -DLWIP_IPV4=1
CFLAGS += -mcpu=cortex-m3 -mthumb -g2 -w $(OPT) -std=gnu99
CFLAGS += -fno-common -fmessage-length=0 -ffunction-sections -fdata-sections -fomit-frame-pointer -fno-short-enums -fsigned-char 
CFLAGS += -Wno-pointer-sign
CFLAGS += -DUSE_AMEBA_STARTUP=$(USE_AMEBA_STARTUP)

INCFLAGS =
INCFLAGS += $(patsubst %,-I%,$(INCLUDES_USER))
INCFLAGS += $(patsubst %,-I%,$(patsubst sdk/%,$(AMEBA1_SDK_PATH)%,$(INCLUDES)))

LFLAGS =
LFLAGS += -mcpu=cortex-m3 -mthumb -g $(OPT) -nostartfiles --specs=nano.specs
LFLAGS += -Wl,--gc-sections -Wl,--cref -Wl,--entry=Reset_Handler -Wl,--no-enum-size-warning -Wl,--no-wchar-size-warning -Wl,-nostdlib -Wl,--no-check-sections
LFLAGS += -Wl,-Map=$(OBJ_DIR)/$(TARGET).map
LFLAGS += -Wl,-wrap,malloc -Wl,-wrap,free -Wl,-wrap,realloc
LFLAGS += -Wl,-wrap,rom_psk_CalcGTK
LFLAGS += -Wl,-wrap,rom_psk_CalcPTK
LFLAGS += -Wl,-wrap,aes_80211_encrypt
LFLAGS += -Wl,-wrap,aes_80211_decrypt
LFLAGS += -DUSE_AMEBA_STARTUP=$(USE_AMEBA_STARTUP)

LIBFLAGS = -L$(AMEBA1_SDK_PATH)soc/realtek/8195a/misc/bsp/lib/common/GCC
# -l_platform -l_platform_lite -l_mdns -l_xmodem -l_dct -l_websocket -l_wps -l_p2p
# -lm -lc -lnosys -lgcc -l_wlan -l_platform_lite -l_rtlstd
ifeq ($(USE_AMEBA_STARTUP), 1)
all: LIBFLAGS += -l_platform_boot_lite -l_wlan -l_http -l_wps -l_p2p -l_rtlstd -l_websocket -l_xmodem -lm -lc -lnosys -lgcc -l_mdns
mp: LIBFLAGS += -l_wlan_mp
else
all: LIBFLAGS += -l_platform_lite -l_wlan -l_http -l_wps -l_p2p -l_rtlstd -l_websocket -l_xmodem -lm -lc -lnosys -lgcc -l_mdns
mp: LIBFLAGS += -l_wlan_mp
endif
#LIBFLAGS += -l_platform_lite -l_wps -l_websocket -l_xmodem -lgcc -lm -lc -lnosys

RAMALL_BIN =
OTA_BIN = 
all: RAMALL_BIN = ram_all.bin
all: OTA_BIN = ota.bin
mp: RAMALL_BIN = ram_all_mp.bin
mp: OTA_BIN = ota_mp.bin

# ---------------------------------------------------- Compile ----------------------------------------------------
# ---------------------------------------------------- Debug ----------------------------------------------------

include $(AMEBA1_SDK_PATH)sdk_compile_a.mk
include $(AMEBA1_SDK_PATH)sdk_debug_a.mk
include $(AMEBA1_SDK_PATH)sdk_flash_a.mk
