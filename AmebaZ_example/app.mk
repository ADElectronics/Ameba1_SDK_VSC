include app_settings.mk
include $(AMEBA1_SDK_PATH)sdk_tools.mk
include $(AMEBA1_SDK_PATH)sdk_sources_z_main.mk
include $(AMEBA1_SDK_PATH)sdk_sources_z_app.mk

# ---------------------------------------------------- Include folder list ----------------------------------------------------

INCLUDES_USER =
INCLUDES_USER += Src
INCLUDES_USER += Inc

# ---------------------------------------------------- Source file list ----------------------------------------------------

SRC_USER_C =
SRC_USER_C += Src/main.c

# ---------------------------------------------------- Compile options ----------------------------------------------------

LDFILE = rlx8711B-symbol-v02-img2_xip${OTA_INDEX}.ld
OPT = -Os

CFLAGS =
CFLAGS += -DGCC_ARMCM4 -DCONFIG_PLATFORM_8711B -DF_CPU=125000000L
CFLAGS += -DLWIP_TIMEVAL_PRIVATE=0 -DLWIP_NO_STDINT_H=1 -DLWIP_NO_CTYPE_H=1 -DIN_ADDR_T_DEFINED=1 -DLWIP_IPV4=1
CFLAGS += -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -g2 -w $(OPT) -std=gnu99
CFLAGS += -Wdouble-promotion -fsingle-precision-constant
CFLAGS += -fno-common -fmessage-length=0 -ffunction-sections -fdata-sections -fomit-frame-pointer -fno-short-enums -fsigned-char
CFLAGS += -Wno-pointer-sign
#CFLAGS += -Wall -Werror

INCFLAGS =
INCFLAGS += $(patsubst %,-I%,$(INCLUDES_USER))
INCFLAGS += $(patsubst %,-I%,$(patsubst sdk/%,$(AMEBA1_SDK_PATH)%,$(INCLUDES)))

LFLAGS = 
LFLAGS += -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -g $(OPT) --specs=nano.specs -nostartfiles
LFLAGS += -Wl,-Map=$(BIN_DIR)/$(TARGET).map
LFLAGS += -Wl,--gc-sections -Wl,--cref -Wl,--entry=Reset_Handler -Wl,--no-enum-size-warning -Wl,--no-wchar-size-warning
LFLAGS += -Wl,-wrap,malloc -Wl,-wrap,free -Wl,-wrap,realloc
LFLAGS += -Wl,-wrap,rom_psk_CalcGTK
LFLAGS += -Wl,-wrap,rom_psk_CalcPTK
LFLAGS += -Wl,-wrap,CalcMIC
LFLAGS += -Wl,-wrap,CheckMIC
LFLAGS += -Wl,-wrap,aes_80211_encrypt
LFLAGS += -Wl,-wrap,aes_80211_decrypt
LFLAGS += -Wl,-wrap,DecGTK

LIBFLAGS = -L$(AMEBA1_SDK_PATH)soc/realtek/8711b/misc/bsp/lib/common/GCC
# -l_platform -l_wlan -l_wlan_mp -l_wps -l_p2p -l_dct -l_rtlstd -lm -lc -lnosys -lgcc -l_websocket -l_http -l_mdns
LIBFLAGS += -l_platform -l_wlan -l_wps -l_p2p -l_dct -l_rtlstd -lm -lc -lnosys -lgcc -l_websocket -l_http -l_mdns

#all:
#mp: 

RAMALL_BIN = ram_all.bin
OTA_BIN = ota.bin
IMAGE2_OTA1 = image2_all_ota1.bin
IMAGE2_OTA2 = image2_all_ota2.bin
OTA_ALL = ota_all.bin
BOOT_ALL = boot_all.bin
SYSTEM_BIN = system.bin

mp: RAMALL_BIN = ram_all_mp.bin
mp: OTA_BIN = ota_mp.bin
mp: IMAGE2_OTA1 = image2_all_ota1_mp.bin
mp: IMAGE2_OTA2 = image2_all_ota2_mp.bin
mp: OTA_ALL = ota_all_mp.bin

# ---------------------------------------------------- Compile ----------------------------------------------------
# ---------------------------------------------------- Debug ----------------------------------------------------

include $(AMEBA1_SDK_PATH)sdk_compile_z.mk
include $(AMEBA1_SDK_PATH)sdk_debug_z.mk
include $(AMEBA1_SDK_PATH)sdk_flash_z.mk
