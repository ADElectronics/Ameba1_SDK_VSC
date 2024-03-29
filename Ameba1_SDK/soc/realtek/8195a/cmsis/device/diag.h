/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2013 Realtek Semiconductor Corp.
 *
 */

#ifndef _DIAG_H_
#define _DIAG_H_

#include "basic_types.h"
#include "platform_autoconf.h"

#include <stddef.h> /* for size_t */

extern u32 ConfigDebugErr;
extern u32 ConfigDebugInfo;
extern u32 ConfigDebugWarn;

extern u32 CfgSysDebugErr;
extern u32 CfgSysDebugInfo;
extern u32 CfgSysDebugWarn;

#define DBG_ERR_MSG_ON(x)       (ConfigDebugErr |= (x))
#define DBG_WARN_MSG_ON(x)      (ConfigDebugWarn |= (x))
#define DBG_INFO_MSG_ON(x)      (ConfigDebugInfo |= (x))

#define DBG_ERR_MSG_OFF(x)      (ConfigDebugErr &= ~(x))
#define DBG_WARN_MSG_OFF(x)     (ConfigDebugWarn &= ~(x))
#define DBG_INFO_MSG_OFF(x)     (ConfigDebugInfo &= ~(x))

// Define debug group
#define	_DBG_BOOT_          	0x00000001
#define	_DBG_GDMA_          	0x00000002
#define	_DBG_GPIO_			    0x00000004
#define	_DBG_TIMER_         	0x00000008
#define	_DBG_I2C_           	0x00000010
#define	_DBG_I2S_           	0x00000020
#define	_DBG_MII_           	0x00000040
#define	_DBG_NFC_           	0x00000080
#define	_DBG_PCM_           	0x00000100
#define	_DBG_PWM_           	0x00000200
#define	_DBG_SDIO_          	0x00000400
#define	_DBG_SSI_           	0x00000800
#define	_DBG_SPI_FLASH_         0x00001000
#define	_DBG_SDR_           	0x00002000
#define	_DBG_UART_          	0x00004000
#define	_DBG_USB_OTG_           0x00008000
#define	_DBG_USB_CORE_          0x00010000
#define	_DBG_CRYPTO_        	0x00020000
#define	_DBG_ADC_           	0x00040000
#define	_DBG_DAC_               0x00080000
#define	_DBG_TCM_HEAP_          0x00100000
#define	_DBG_RAM_HEAP_          0x00200000
#define	_DBG_FEEP_          	0x00400000

#define _DBG_MISC_              0x40000000
#define	_DBG_FAULT_        	    0x80000000

typedef enum _SYSTEM_DBG_DEFINE_ {
    _SYSDBG_MISC_       = 1<<0,
    _SYSDBG_MAILBOX_    = 1<<1,
    _SYSDBG_TIMER_      = 1<<2

} SYSTEM_DBG;

extern
_LONG_CALL_ROM_ u32
DiagPrintf(
    IN  const char *fmt, ...
);

u32
DiagSPrintf(
    IN  u8 *buf,
    IN  const char *fmt, ...
);

int
prvDiagPrintf(
    IN  const char *fmt, ...
);

int
prvDiagSPrintf(
    IN char *buf,
    IN  const char *fmt, ...
);

extern char print_off;

#if CONFIG_DEBUG_LOG > 3
#define debug_printf(fmt, ...) rtl_printf(fmt, ##__VA_ARGS__)
#define info_printf(fmt, ...) rtl_printf(fmt, ##__VA_ARGS__)
#define warning_printf(fmt, ...) rtl_printf(fmt, ##__VA_ARGS__)
#define error_printf(fmt, ...) rtl_printf(fmt, ##__VA_ARGS__)
#elif CONFIG_DEBUG_LOG > 2
#define debug_printf(fmt, ...)
#define info_printf(fmt, ...) rtl_printf(fmt, ##__VA_ARGS__)
#define warning_printf(fmt, ...) rtl_printf(fmt, ##__VA_ARGS__)
#define error_printf(fmt, ...) rtl_printf(fmt, ##__VA_ARGS__)
#elif CONFIG_DEBUG_LOG > 1
#define debug_printf(fmt, ...)
#define info_printf(fmt, ...)
#define warning_printf(fmt, ...) rtl_printf(fmt, ##__VA_ARGS__)
#define error_printf(fmt, ...) rtl_printf(fmt, ##__VA_ARGS__)
#elif CONFIG_DEBUG_LOG > 0
#define debug_printf(fmt, ...)
#define info_printf(fmt, ...)
#define warning_printf(fmt, ...)
#define error_printf(fmt, ...) rtl_printf(fmt, ##__VA_ARGS__)
#else
#define debug_printf(fmt, ...)
#define info_printf(fmt, ...)
#define warning_printf(fmt, ...)
#define error_printf(fmt, ...)
#endif

#define _DbgDump  DiagPrintf

#define DRIVER_PREFIX			"RTL8195A[Driver] "
#define HAL_PREFIX				"RTL8195A[HAL] "
#define DMA_PREFIX				"RTL8195A[DMA] "
#define SDIO_PREFIX				"RTL8195A[SDIO] "
#define MBOX_PREFIX				"[OS-MBOX] "
#define TIMER_PREFIX			"[OS-TMR] "

#define BOOT_ERR_PREFIX         "[BOOT Err] "
#define BOOT_WARN_PREFIX        "[BOOT Wrn] "
#define BOOT_INFO_PREFIX        "[BOOT Inf] "

#define GDMA_ERR_PREFIX         "[GDMA Err] "
#define GDMA_WARN_PREFIX        "[GDMA Wrn] "
#define GDMA_INFO_PREFIX        "[GDMA Inf] "

#define GPIO_ERR_PREFIX         "[GPIO Err] "
#define GPIO_WARN_PREFIX        "[GPIO Wrn] "
#define GPIO_INFO_PREFIX        "[GPIO Inf] "

#define TIMER_ERR_PREFIX        "[TIMR Err] "
#define TIMER_WARN_PREFIX       "[TIMR Wrn] "
#define TIMER_INFO_PREFIX       "[TIMR Inf] "

#define I2C_ERR_PREFIX          "[I2C  Err] "
#define I2C_WARN_PREFIX         "[I2C  Wrn] "
#define I2C_INFO_PREFIX         "[I2C  Inf] "

#define I2S_ERR_PREFIX          "[I2S  Err] "
#define I2S_WARN_PREFIX         "[I2S  Wrn] "
#define I2S_INFO_PREFIX         "[I2S  Inf] "

#define MII_ERR_PREFIX          "[MII  Err] "
#define MII_WARN_PREFIX         "[MII  Wrn] "
#define MII_INFO_PREFIX         "[MII  Inf] "

#define NFC_ERR_PREFIX          "[NFC  Err] "
#define NFC_WARN_PREFIX         "[NFC  Wrn] "
#define NFC_INFO_PREFIX         "[NFC  Inf] "

#define PCM_ERR_PREFIX          "[PCM  Err] "
#define PCM_WARN_PREFIX         "[PCM  Wrn] "
#define PCM_INFO_PREFIX         "[PCM  Inf] "

#define PWM_ERR_PREFIX          "[PWM  Err] "
#define PWM_WARN_PREFIX         "[PWM  Wrn] "
#define PWM_INFO_PREFIX         "[PWM  Inf] "

#define SSI_ERR_PREFIX          "[SSI  Err] "
#define SSI_WARN_PREFIX         "[SSI  Wrn] "
#define SSI_INFO_PREFIX         "[SSI  Inf] "

#define SDIO_ERR_PREFIX         "[SDIO Err] "
#define SDIO_WARN_PREFIX        "[SDIO Wrn] "
#define SDIO_INFO_PREFIX        "[SDIO Inf] "

#define SPIF_ERR_PREFIX         "[SPIF Err] "
#define SPIF_WARN_PREFIX        "[SPIF Wrn] "
#define SPIF_INFO_PREFIX        "[SPIF Inf] "

#define SDR_ERR_PREFIX          "[SDR  Err] "
#define SDR_WARN_PREFIX         "[SDR  Wrn] "
#define SDR_INFO_PREFIX         "[SDR  Inf] "

#define UART_ERR_PREFIX         "[UART Err] "
#define UART_WARN_PREFIX        "[UART Wrn] "
#define UART_INFO_PREFIX        "[UART Inf] "

#define USB_ERR_PREFIX          "[USB  Err] "
#define USB_WARN_PREFIX         "[USB  Wrn] "
#define USB_INFO_PREFIX         "[USB  Inf] "

#define IPSEC_ERR_PREFIX        "[CRYP Err] "
#define IPSEC_WARN_PREFIX       "[CRYP Wrn] "
#define IPSEC_INFO_PREFIX       "[CRYP Inf] "

#define ADC_ERR_PREFIX          "[ADC  Err] "
#define ADC_WARN_PREFIX         "[ADC  Wrn] "
#define ADC_INFO_PREFIX         "[ADC  Inf] "

#define DAC_ERR_PREFIX          "[DAC  Err] "
#define DAC_WARN_PREFIX         "[DAC  Wrn] "
#define DAC_INFO_PREFIX         "[DAC  Inf] "

#define MISC_ERR_PREFIX         "[MISC Err] "
#define MISC_WARN_PREFIX        "[MISC Wrn] "
#define MISC_INFO_PREFIX        "[MISC Inf] "

#define OTG_ERR_PREFIX			"[OTG  Err] "
#define OTG_WARN_PREFIX			"[OTG  Wrn] "
#define OTG_INFO_PREFIX			"[OTG  Inf] "

#define HEAP_ERR_PREFIX         "[HEAP Err] "
#define HEAP_WARN_PREFIX        "[HEAP Wrn] "
#define HEAP_INFO_PREFIX        "[HEAP Inf] "

#define FEEP_ERR_PREFIX         "[FEEP Err] "
#define FEEP_WARN_PREFIX        "[FEEP Wrn] "
#define FEEP_INFO_PREFIX        "[FEEP Inf] "

#define OTG_PREFIX				"RTL8195A[OTG] "
#define OTG_PREFIX_LVL			"RTL8195A[OTG_LVL_%2x] "

#ifdef CONFIG_DEBUG_LOG
 #if CONFIG_DEBUG_LOG > 2
	#define CONFIG_DEBUG_ERROR      1
	#define CONFIG_DEBUG_WARN       1
	#define CONFIG_DEBUG_INFO       1
 #elif CONFIG_DEBUG_LOG > 1
	#define CONFIG_DEBUG_ERROR      1
	#define CONFIG_DEBUG_WARN       1
 #elif CONFIG_DEBUG_LOG > 0
	#define CONFIG_DEBUG_ERROR      1
 #endif
#endif

#ifndef likely
#define likely(x)               (x)
#define unlikely(x)             (x)
#endif


#if CONFIG_DEBUG_ERROR      // if Build-In Debug Error Message

#define DBG_BOOT_ERR(...)     do {\
    if (likely(ConfigDebugErr & _DBG_BOOT_)) \
        _DbgDump(BOOT_ERR_PREFIX __VA_ARGS__);\
}while(0)

#define DBG_GDMA_ERR(...)     do {\
    if (likely(ConfigDebugErr & _DBG_GDMA_)) \
        _DbgDump(GDMA_ERR_PREFIX __VA_ARGS__);\
}while(0)

#define DBG_GPIO_ERR(...)     do {\
    if (likely(ConfigDebugErr & _DBG_GPIO_)) \
        _DbgDump(GPIO_ERR_PREFIX __VA_ARGS__);\
}while(0)

#define DBG_TIMER_ERR(...)     do {\
    if (likely(ConfigDebugErr & _DBG_TIMER_)) \
        _DbgDump(TIMER_ERR_PREFIX __VA_ARGS__);\
}while(0)

#define DBG_I2C_ERR(...)     do {\
    if (likely(ConfigDebugErr & _DBG_I2C_)) \
        _DbgDump(I2C_ERR_PREFIX __VA_ARGS__);\
}while(0)

#define DBG_I2S_ERR(...)     do {\
    if (likely(ConfigDebugErr & _DBG_I2S_)) \
        _DbgDump(I2S_ERR_PREFIX __VA_ARGS__);\
}while(0)

#define DBG_MII_ERR(...)     do {\
    if (likely(ConfigDebugErr & _DBG_MII_)) \
        _DbgDump(MII_ERR_PREFIX __VA_ARGS__);\
}while(0)

#define DBG_NFC_ERR(...)     do {\
    if (likely(ConfigDebugErr & _DBG_NFC_)) \
        _DbgDump(NFC_ERR_PREFIX __VA_ARGS__);\
}while(0)

#define DBG_PCM_ERR(...)     do {\
    if (likely(ConfigDebugErr & _DBG_PCM_)) \
        _DbgDump(PCM_ERR_PREFIX __VA_ARGS__);\
}while(0)

#define DBG_PWM_ERR(...)     do {\
    if (likely(ConfigDebugErr & _DBG_PWM_)) \
        _DbgDump(PWM_ERR_PREFIX __VA_ARGS__);\
}while(0)

#define DBG_SSI_ERR(...)     do {\
    if (likely(ConfigDebugErr & _DBG_SSI_)) \
        _DbgDump(SSI_ERR_PREFIX __VA_ARGS__);\
}while(0)

#define DBG_SDIO_ERR(...)     do {\
    if (likely(ConfigDebugErr & _DBG_SDIO_)) \
        _DbgDump(SDIO_ERR_PREFIX __VA_ARGS__);\
}while(0)

#define DBG_SPIF_ERR(...)     do {\
    if (likely(ConfigDebugErr & _DBG_SPI_FLASH_)) \
        _DbgDump(SPIF_ERR_PREFIX __VA_ARGS__);\
}while(0)

#define DBG_SDR_ERR(...)     do {\
    if (likely(ConfigDebugErr & _DBG_SDR_)) \
        _DbgDump(SDR_ERR_PREFIX __VA_ARGS__);\
}while(0)

#define DBG_UART_ERR(...)     do {\
    if (likely(ConfigDebugErr & _DBG_UART_)) \
        _DbgDump(UART_ERR_PREFIX __VA_ARGS__);\
}while(0)

#define DBG_USBOTG_ERR(...)     do {\
    if (likely(ConfigDebugErr & _DBG_USB_OTG_)) \
        _DbgDump( __VA_ARGS__);\
}while(0)

#define DBG_USBCOR_ERR(...)     do {\
    if (likely(ConfigDebugErr & _DBG_USB_CORE_)) \
        _DbgDump(USB_ERR_PREFIX __VA_ARGS__);\
}while(0)

#define DBG_CRYPTO_ERR(...)     do {\
    if (likely(ConfigDebugErr & _DBG_CRYPTO_)) \
        _DbgDump(IPSEC_ERR_PREFIX __VA_ARGS__);\
}while(0)

#define DBG_ADC_ERR(...)     do {\
    if (likely(ConfigDebugErr & _DBG_ADC_)) \
        _DbgDump(ADC_ERR_PREFIX __VA_ARGS__);\
}while(0)

#define DBG_DAC_ERR(...)     do {\
    if (likely(ConfigDebugErr & _DBG_DAC_)) \
        _DbgDump(DAC_ERR_PREFIX __VA_ARGS__);\
}while(0)

#define MSG_MBOX_ERR(...)     do {\
    if (likely(CfgSysDebugErr & _SYSDBG_MAILBOX_)) \
        _DbgDump(MBOX_PREFIX __VA_ARGS__);\
}while(0)

#define MSG_TIMER_ERR(...)     do {\
    if (likely(CfgSysDebugErr & _SYSDBG_TIMER_)) \
        _DbgDump(TIMER_PREFIX __VA_ARGS__);\
}while(0)

#define DBG_8195A_OTG(...)  do{\
	if (unlikely(ConfigDebugInfo & _DBG_USB_OTG_)) \
		_DbgDump(OTG_PREFIX __VA_ARGS__);\
	}while(0)

#define DBG_8195A_OTG_INFO(...)  do{\
	if (unlikely(ConfigDebugInfo & _DBG_USB_OTG_)) \
		_DbgDump(OTG_PREFIX __VA_ARGS__);\
	}while(0)

#define DBG_8195A_OTG_WARN(...)  do{\
	if (unlikely(ConfigDebugWarn & _DBG_USB_OTG_)) \
		_DbgDump(OTG_PREFIX __VA_ARGS__);\
	}while(0)

#define DBG_8195A_OTG_ERR(...)  do{\
	if (unlikely(ConfigDebugErr & _DBG_USB_OTG_)) \
		_DbgDump(OTG_PREFIX __VA_ARGS__);\
	}while(0)
            
#define DBG_8195A_OTG_LVL(LVL,...)  do{\
    if (unlikely(ConfigDebugInfo & _DBG_USB_OTG_)){ \
        _DbgDump(OTG_PREFIX_LVL,LVL);\
        _DbgDump(__VA_ARGS__);\
    }\
}while(0)

#define DBG_TCM_HEAP_ERR(...)     do {\
    if (likely(ConfigDebugErr & _DBG_TCM_HEAP_)){ \
    _DbgDump(HEAP_ERR_PREFIX ANSI_COLOR_RED);\
    _DbgDump(__VA_ARGS__ );\
    _DbgDump(ANSI_COLOR_RESET);}\
}while(0)

#define DBG_RAM_HEAP_ERR(...)     do {\
    if (likely(ConfigDebugErr & _DBG_RAM_HEAP_)){ \
    _DbgDump(HEAP_ERR_PREFIX ANSI_COLOR_RED);\
    _DbgDump(__VA_ARGS__ );\
    _DbgDump(ANSI_COLOR_RESET);}\
}while(0)

#define DBG_MISC_ERR(...)     do {\
    if (likely(ConfigDebugErr & _DBG_MISC_)) \
        _DbgDump(MISC_ERR_PREFIX __VA_ARGS__);\
}while(0)

#define DBG_FEEP_ERR(...)     do {\
    if (likely(ConfigDebugErr & _DBG_FEEP_)) \
        _DbgDump(FEEP_ERR_PREFIX __VA_ARGS__);\
}while(0)


#else   // else of "#if CONFIG_DEBUG_ERROR"

#define DBG_BOOT_ERR(...)
#define DBG_GDMA_ERR(...)
#define DBG_GPIO_ERR(...)
#define DBG_TIMER_ERR(...)
#define DBG_I2C_ERR(...)
#define DBG_I2S_ERR(...)
#define DBG_MII_ERR(...)
#define DBG_NFC_ERR(...)
#define DBG_PCM_ERR(...)
#define DBG_PWM_ERR(...)
#define DBG_SSI_ERR(...)
#define DBG_SDIO_ERR(...)
#define DBG_SPIF_ERR(...)
#define DBG_SDR_ERR(...)
#define DBG_UART_ERR(...)
#define DBG_USBOTG_ERR(...)
#define DBG_USBCOR_ERR(...)
#define DBG_CRYPTO_ERR(...)
#define DBG_ADC_ERR(...)
#define DBG_DAC_ERR(...)

#define MSG_MBOX_ERR(...)
#define MSG_TIMER_ERR(...)
#define DBG_8195A_OTG(...)
#define DBG_8195A_OTG_LVL(LVL,...)
#define DBG_8195A_OTG_INFO(...)
#define DBG_8195A_OTG_WARN(...)
#define DBG_8195A_OTG_ERR(...)

#define DBG_TCM_HEAP_ERR(...)
#define DBG_RAM_HEAP_ERR(...)
#define DBG_FEEP_ERR(...)
#define DBG_MISC_ERR(...)

#endif  // end of else of "#if CONFIG_DEBUG_ERROR"

// =============================================================

#if CONFIG_DEBUG_WARN     // if Build-In Debug Warring Message

#define DBG_BOOT_WARN(...)     do {\
    if (unlikely(ConfigDebugWarn& _DBG_BOOT_)) \
        _DbgDump(BOOT_WARN_PREFIX __VA_ARGS__);\
}while(0)

#define DBG_GDMA_WARN(...)     do {\
    if (unlikely(ConfigDebugWarn & _DBG_GDMA_)) \
        _DbgDump(GDMA_WARN_PREFIX __VA_ARGS__);\
}while(0)

#define DBG_GPIO_WARN(...)     do {\
    if (unlikely(ConfigDebugWarn & _DBG_GPIO_)) \
        _DbgDump(GPIO_WARN_PREFIX __VA_ARGS__);\
}while(0)

#define DBG_TIMER_WARN(...)     do {\
    if (unlikely(ConfigDebugWarn & _DBG_TIMER_)) \
        _DbgDump(TIMER_WARN_PREFIX __VA_ARGS__);\
}while(0)

#define DBG_I2C_WARN(...)     do {\
    if (unlikely(ConfigDebugWarn & _DBG_I2C_)) \
        _DbgDump(I2C_WARN_PREFIX __VA_ARGS__);\
}while(0)

#define DBG_I2S_WARN(...)     do {\
    if (unlikely(ConfigDebugWarn & _DBG_I2S_)) \
        _DbgDump(I2S_WARN_PREFIX __VA_ARGS__);\
}while(0)

#define DBG_MII_WARN(...)     do {\
    if (unlikely(ConfigDebugWarn & _DBG_MII_)) \
        _DbgDump(MII_WARN_PREFIX __VA_ARGS__);\
}while(0)

#define DBG_NFC_WARN(...)     do {\
    if (unlikely(ConfigDebugWarn & _DBG_NFC_)) \
        _DbgDump(NFC_WARN_PREFIX __VA_ARGS__);\
}while(0)

#define DBG_PCM_WARN(...)     do {\
    if (unlikely(ConfigDebugWarn & _DBG_PCM_)) \
        _DbgDump(PCM_WARN_PREFIX __VA_ARGS__);\
}while(0)

#define DBG_PWM_WARN(...)     do {\
    if (unlikely(ConfigDebugWarn & _DBG_PWM_)) \
        _DbgDump(PWM_WARN_PREFIX __VA_ARGS__);\
}while(0)

#define DBG_SSI_WARN(...)     do {\
    if (unlikely(ConfigDebugWarn & _DBG_SSI_)) \
        _DbgDump(SSI_WARN_PREFIX __VA_ARGS__);\
}while(0)

#define DBG_SDIO_WARN(...)     do {\
    if (unlikely(ConfigDebugWarn & _DBG_SDIO_)) \
        _DbgDump(SDIO_WARN_PREFIX __VA_ARGS__);\
}while(0)

#define DBG_SPIF_WARN(...)     do {\
    if (unlikely(ConfigDebugWarn & _DBG_SPI_FLASH_)) \
        _DbgDump(SPIF_WARN_PREFIX __VA_ARGS__);\
}while(0)

#define DBG_SDR_WARN(...)     do {\
    if (unlikely(ConfigDebugWarn & _DBG_SDR_)) \
        _DbgDump(SDR_WARN_PREFIX __VA_ARGS__);\
}while(0)

#define DBG_UART_WARN(...)     do {\
    if (unlikely(ConfigDebugWarn & _DBG_UART_)) \
        _DbgDump(UART_WARN_PREFIX __VA_ARGS__);\
}while(0)

#define DBG_USBOTG_WARN(...)     do {\
    if (unlikely(ConfigDebugWarn & _DBG_USB_OTG_)) \
        _DbgDump( __VA_ARGS__);\
}while(0)

#define DBG_USBCOR_WARN(...)     do {\
    if (unlikely(ConfigDebugWarn & _DBG_USB_CORE_)) \
        _DbgDump(USB_WARN_PREFIX __VA_ARGS__);\
}while(0)

#define DBG_CRYPTO_WARN(...)     do {\
    if (unlikely(ConfigDebugWarn & _DBG_CRYPTO_)) \
        _DbgDump(IPSEC_WARN_PREFIX __VA_ARGS__);\
}while(0)

#define DBG_ADC_WARN(...)     do {\
    if (unlikely(ConfigDebugWarn & _DBG_ADC_)) \
        _DbgDump(ADC_WARN_PREFIX __VA_ARGS__);\
}while(0)

#define DBG_DAC_WARN(...)     do {\
    if (unlikely(ConfigDebugWarn & _DBG_DAC_)) \
        _DbgDump(DAC_WARN_PREFIX __VA_ARGS__);\
}while(0)

#define MSG_MBOX_WARN(...)     do {\
    if (unlikely(CfgSysDebugWarn& _SYSDBG_MAILBOX_)) \
        _DbgDump(MBOX_PREFIX __VA_ARGS__);\
}while(0)

#define MSG_TIMER_WARN(...)     do {\
    if (unlikely(CfgSysDebugWarn & _SYSDBG_TIMER_)) \
        _DbgDump(TIMER_PREFIX __VA_ARGS__);\
}while(0)

#define DBG_TCM_HEAP_WARN(...)     do {\
    if (likely(ConfigDebugWarn & _DBG_TCM_HEAP_)){ \
        _DbgDump(HEAP_WARN_PREFIX ANSI_COLOR_MAGENTA);\
        _DbgDump(__VA_ARGS__ );\
        _DbgDump(ANSI_COLOR_RESET);}\
}while(0)

#define DBG_RAM_HEAP_WARN(...)     do {\
    if (likely(ConfigDebugWarn & _DBG_RAM_HEAP_)){ \
        _DbgDump(HEAP_WARN_PREFIX ANSI_COLOR_MAGENTA);\
        _DbgDump(__VA_ARGS__ );\
        _DbgDump(ANSI_COLOR_RESET);}\
}while(0)

#define DBG_FEEP_WARN(...)     do {\
    if (likely(ConfigDebugWarn & _DBG_FEEP_)) \
        _DbgDump(FEEP_WARN_PREFIX __VA_ARGS__);\
}while(0)

#define DBG_MISC_WARN(...)     do {\
    if (likely(ConfigDebugWarn & _DBG_MISC_)) \
        _DbgDump(MISC_WARN_PREFIX __VA_ARGS__);\
}while(0)

#else   // else of "#if CONFIG_DEBUG_WARN"

#define DBG_BOOT_WARN(...)
#define DBG_GDMA_WARN(...)
#define DBG_GPIO_WARN(...)
#define DBG_TIMER_WARN(...)
#define DBG_I2C_WARN(...)
#define DBG_I2S_WARN(...)
#define DBG_MII_WARN(...)
#define DBG_NFC_WARN(...)
#define DBG_PCM_WARN(...)
#define DBG_PWM_WARN(...)
#define DBG_SSI_WARN(...)
#define DBG_SDIO_WARN(...)
#define DBG_SPIF_WARN(...)
#define DBG_SDR_WARN(...)
#define DBG_UART_WARN(...)
#define DBG_USBOTG_WARN(...)
#define DBG_USBCOR_WARN(...)
#define DBG_CRYPTO_WARN(...)
#define DBG_ADC_WARN(...)
#define DBG_DAC_WARN(...)
#define DBG_TCM_HEAP_WARN(...)
#define DBG_RAM_HEAP_WARN(...)
#define DBG_FEEP_WARN(...)
#define DBG_MISC_WARN(...)

#define MSG_MBOX_WARN(...)
#define MSG_TIMER_WARN(...)

#endif  // end of else of "#if CONFIG_DEBUG_WARN"

// =============================================================

#if CONFIG_DEBUG_INFO     // if Build-In Debug Information Message

#define DBG_BOOT_INFO(...)     do {\
    if (unlikely(ConfigDebugInfo & _DBG_BOOT_)) \
        _DbgDump(BOOT_INFO_PREFIX __VA_ARGS__);\
}while(0)

#define DBG_GDMA_INFO(...)     do {\
    if (unlikely(ConfigDebugInfo & _DBG_GDMA_)) \
        _DbgDump(GDMA_INFO_PREFIX __VA_ARGS__);\
}while(0)

#define DBG_GPIO_INFO(...)     do {\
    if (unlikely(ConfigDebugInfo & _DBG_GPIO_)) \
        _DbgDump(GPIO_INFO_PREFIX __VA_ARGS__);\
}while(0)

#define DBG_TIMER_INFO(...)     do {\
    if (unlikely(ConfigDebugInfo & _DBG_TIMER_)) \
        _DbgDump(TIMER_INFO_PREFIX __VA_ARGS__);\
}while(0)

#define DBG_I2C_INFO(...)     do {\
    if (unlikely(ConfigDebugInfo & _DBG_I2C_)) \
        _DbgDump(I2C_INFO_PREFIX __VA_ARGS__);\
}while(0)

#define DBG_I2S_INFO(...)     do {\
    if (unlikely(ConfigDebugInfo & _DBG_I2S_)) \
        _DbgDump(I2S_INFO_PREFIX __VA_ARGS__);\
}while(0)

#define DBG_MII_INFO(...)     do {\
    if (unlikely(ConfigDebugInfo & _DBG_MII_)) \
        _DbgDump(MII_INFO_PREFIX __VA_ARGS__);\
}while(0)

#define DBG_NFC_INFO(...)     do {\
    if (unlikely(ConfigDebugInfo & _DBG_NFC_)) \
        _DbgDump(NFC_INFO_PREFIX __VA_ARGS__);\
}while(0)

#define DBG_PCM_INFO(...)     do {\
    if (unlikely(ConfigDebugInfo & _DBG_PCM_)) \
        _DbgDump(PCM_INFO_PREFIX __VA_ARGS__);\
}while(0)

#define DBG_PWM_INFO(...)     do {\
    if (unlikely(ConfigDebugInfo & _DBG_PWM_)) \
        _DbgDump(PWM_INFO_PREFIX __VA_ARGS__);\
}while(0)

#define DBG_SSI_INFO(...)     do {\
    if (unlikely(ConfigDebugInfo & _DBG_SSI_)) \
        _DbgDump(SSI_INFO_PREFIX __VA_ARGS__);\
}while(0)

#define DBG_SDIO_INFO(...)     do {\
    if (unlikely(ConfigDebugInfo & _DBG_SDIO_)) \
        _DbgDump(SDIO_INFO_PREFIX __VA_ARGS__);\
}while(0)

#define DBG_SPIF_INFO(...)     do {\
    if (unlikely(ConfigDebugInfo & _DBG_SPI_FLASH_)) \
        _DbgDump(SPIF_INFO_PREFIX __VA_ARGS__);\
}while(0)

#define DBG_SDR_INFO(...)     do {\
    if (unlikely(ConfigDebugInfo & _DBG_SDR_)) \
        _DbgDump(SDR_INFO_PREFIX __VA_ARGS__);\
}while(0)

#define DBG_UART_INFO(...)     do {\
    if (unlikely(ConfigDebugInfo & _DBG_UART_)) \
        _DbgDump(UART_INFO_PREFIX __VA_ARGS__);\
}while(0)

#define DBG_USBOTG_INFO(...)     do {\
    if (unlikely(ConfigDebugInfo & _DBG_USB_OTG_)) \
        _DbgDump( __VA_ARGS__);\
}while(0)

#define DBG_USBCOR_INFO(...)     do {\
    if (unlikely(ConfigDebugInfo & _DBG_USB_CORE_)) \
        _DbgDump(USB_INFO_PREFIX __VA_ARGS__);\
}while(0)

#define DBG_CRYPTO_INFO(...)     do {\
    if (unlikely(ConfigDebugInfo & _DBG_CRYPTO_)) \
        _DbgDump(IPSEC_INFO_PREFIX __VA_ARGS__);\
}while(0)

#define DBG_ADC_INFO(...)     do {\
    if (unlikely(ConfigDebugInfo & _DBG_ADC_)) \
        _DbgDump(ADC_INFO_PREFIX __VA_ARGS__);\
}while(0)

#define DBG_DAC_INFO(...)     do {\
    if (unlikely(ConfigDebugInfo & _DBG_DAC_)) \
        _DbgDump(DAC_INFO_PREFIX __VA_ARGS__);\
}while(0)

#define MSG_MBOX_INFO(...)     do {\
    if (unlikely(CfgSysDebugInfo & _SYSDBG_MAILBOX_)) \
        _DbgDump(MBOX_PREFIX __VA_ARGS__);\
}while(0)

#define MSG_TIMER_INFO(...)     do {\
    if (unlikely(CfgSysDebugInfo & _SYSDBG_TIMER_)) \
        _DbgDump(TIMER_PREFIX __VA_ARGS__);\
}while(0)

#define DBG_TCM_HEAP_INFO(...)     do {\
    if (likely(ConfigDebugInfo & _DBG_TCM_HEAP_)){\
		_DbgDump(HEAP_INFO_PREFIX ANSI_COLOR_BLUE);\
		_DbgDump(__VA_ARGS__ );\
		_DbgDump(ANSI_COLOR_RESET);}\
}while(0)

#define DBG_RAM_HEAP_INFO(...)     do {\
    if (likely(ConfigDebugInfo & _DBG_RAM_HEAP_)){ \
		_DbgDump(HEAP_INFO_PREFIX ANSI_COLOR_BLUE);\
		_DbgDump(__VA_ARGS__ );\
		_DbgDump(ANSI_COLOR_RESET);}\
}while(0)

#define DBG_FEEP_INFO(...)     do {\
    if (unlikely(ConfigDebugInfo & _DBG_FEEP_)) \
        _DbgDump(FEEP_INFO_PREFIX __VA_ARGS__);\
}while(0)

#define DBG_MISC_INFO(...)     do {\
    if (likely(ConfigDebugInfo & _DBG_MISC_)) \
        _DbgDump(MISC_INFO_PREFIX __VA_ARGS__);\
}while(0)

#else   // else of "#if CONFIG_DEBUG_INFO"

#define DBG_BOOT_INFO(...)
#define DBG_GDMA_INFO(...)
#define DBG_GPIO_INFO(...)
#define DBG_TIMER_INFO(...)
#define DBG_I2C_INFO(...)
#define DBG_I2S_INFO(...)
#define DBG_MII_INFO(...)
#define DBG_NFC_INFO(...)
#define DBG_PCM_INFO(...)
#define DBG_PWM_INFO(...)
#define DBG_SSI_INFO(...)
#define DBG_SDIO_INFO(...)
#define DBG_SPIF_INFO(...)
#define DBG_SDR_INFO(...)
#define DBG_UART_INFO(...)
#define DBG_USBOTG_INFO(...)
#define DBG_USBCOR_INFO(...)
#define DBG_CRYPTO_INFO(...)
#define DBG_ADC_INFO(...)
#define DBG_DAC_INFO(...)
#define DBG_RAM_HEAP_INFO(...)
#define DBG_TCM_HEAP_INFO(...)
#define DBG_FEEP_INFO(...)
#define DBG_MISC_INFO(...)

#define MSG_MBOX_INFO(...)
#define MSG_TIMER_INFO(...)

#endif  // end of else of "#if CONFIG_DEBUG_INFO"

// =============================================================

#ifdef CONFIG_DEBUG_LOG

#define DBG_8195A_DRIVER(...)     do {\
    if (unlikely(ConfigDebugErr & (_DBG_I2S_|_DBG_PCM_|_DBG_TIMER_))) \
        _DbgDump(DRIVER_PREFIX __VA_ARGS__);\
}while(0)

#define DBG_8195A_HAL(...)     do {\
    if (unlikely(ConfigDebugErr & (_DBG_SDR_|_DBG_MISC_))) \
        _DbgDump(HAL_PREFIX __VA_ARGS__);\
}while(0)

#define DBG_8195A_DMA(...)     do {\
    if (unlikely(ConfigDebugErr & _DBG_GDMA_)) \
        _DbgDump(DMA_PREFIX __VA_ARGS__);\
}while(0)

#define DBG_8195A_SDIO(...)     do {\
    if (unlikely(ConfigDebugErr & _DBG_SDIO_)) \
        _DbgDump(SDIO_PREFIX __VA_ARGS__);\
}while(0)

#define DBG_8195A(...)     do {\
    if (unlikely(ConfigDebugErr & _DBG_MISC_)) \
        _DbgDump( __VA_ARGS__);\
}while(0)

#define MONITOR_LOG(...)     do {\
    if (unlikely(ConfigDebugErr & _DBG_MISC_)) \
        _DbgDump( __VA_ARGS__);\
}while(0)

#define DBG_ERROR_LOG(...)     do {\
    if (unlikely(ConfigDebugErr & _DBG_FAULT_)) \
        _DbgDump( __VA_ARGS__);\
}while(0)

#ifdef __GNUC__
#define DBG_ASSERT(x)     do {\
        if (unlikely(!(x))) \
            _DbgDump("Assertion: %s:%s, %d\n", __FILE__, __func__, __LINE__);\
    }while(0)
#endif

#ifdef __ICCARM__
#define DBG_ASSERT(x)     do {\
        if (unlikely(!(x))) \
            _DbgDump("Assertion: %s:%s, %d\n", __FILE__, __func__, __LINE__);\
    }while(0)
#endif

#else // #ifdef CONFIG_DEBUG_LOG

#define DBG_8195A_DRIVER(...)
#define DBG_8195A_HAL(...)
#define DBG_8195A_DMA(...)
#define DBG_8195A_SDIO(...)
#define DBG_8195A(...)
#define MONITOR_LOG(...)
#define DBG_ERROR_LOG(...)
#define DBG_ASSERT(...)

#endif // #ifdef CONFIG_DEBUG_LOG

// =============================================================
#if 0
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_RESET   "\x1b[0m"
#else
#define ANSI_COLOR_GREEN	""
#define ANSI_COLOR_CYAN		""
#define ANSI_COLOR_YELLOW	""
#define ANSI_COLOR_MAGENTA	""
#define ANSI_COLOR_RED		""
#define ANSI_COLOR_BLUE		""
#define ANSI_COLOR_RESET	""
#endif

#define IDENT_ONE_SPACE    " "
#define IDENT_TWO_SPACE    "  "
#define IDENT_FOUR_SPACE   "    "
#define IDENT_SIX_SPACE    "      "
#define IDENT_EIGHT_SPACE  "        "

// =============================================================

#ifdef CONFIG_DEBUG_LOG
typedef enum _DBG_CFG_TYPE_ {
	DBG_CFG_ERR=0,
	DBG_CFG_WARN=1,
	DBG_CFG_INFO=2
} DBG_CFG_TYPE;

typedef struct _DBG_CFG_CMD_ {
	u8 cmd_name[16];
	u32	cmd_type;
} DBG_CFG_CMD, *PDBG_CFG_CMD;
#endif // #ifdef CONFIG_DEBUG_LOG

typedef enum _CONSOLE_OP_STAGE_ {
    ROM_STAGE = 0,
    RAM_STAGE = 1
}CONSOLE_OP_STAGE;

#endif //_DIAG_H_
