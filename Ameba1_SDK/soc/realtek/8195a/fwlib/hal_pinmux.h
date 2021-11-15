#ifndef _HAL_PINMUX_
#define _HAL_PINMUX_

//Function Index
#define UART0       0
#define UART1       1
#define UART2       2
#define SPI0        8
#define SPI1        9
#define SPI2        10
#define SPI0_MCS    15
#define I2C0        16
#define I2C1        17
#define I2C2        18
#define I2C3        19
#define I2S0        24
#define I2S1        25
#define PCM0        28
#define PCM1        29
#define ADC0        32
#define DAC0        36
#define DAC1        37
#define SDIOD       64
#define SDIOH       65
#define USBOTG      66
#define MII         88
#define WL_LED      96
#define WL_ANT0     104
#define WL_ANT1     105
#define WL_BTCOEX   108
#define WL_BTCMD    109
#define NFC         112
#define PWM0        160
#define PWM1        161
#define PWM2        162
#define PWM3        163
#define ETE0        164
#define ETE1        165
#define ETE2        166
#define ETE3        167
#define EGTIM       168
#define SPI_FLASH   196
#define SDR         200
#define JTAG        216
#define TRACE       217
#define LOG_UART    220
#define LOG_UART_IR 221
#define SIC         224
#define EEPROM      225
#define DEBUG       226

#if 1
// export-rom_v04.txt
extern _LONG_CALL_ int HalUart0PinCtrlRtl8195A(IN u32 PinLocation, IN BOOL Operation);
extern _LONG_CALL_ int HalUart1PinCtrlRtl8195A(IN u32 PinLocation, IN BOOL Operation);
extern _LONG_CALL_ int HalUart2PinCtrlRtl8195A(IN u32 PinLocation, IN BOOL Operation);
extern _LONG_CALL_ int HalSPI0PinCtrlRtl8195A(IN u32 PinLocation, IN BOOL Operation);
extern _LONG_CALL_ int HalSPI1PinCtrlRtl8195A(IN u32 PinLocation, IN BOOL Operation);
extern _LONG_CALL_ int HalSPI2PinCtrlRtl8195A(IN u32 PinLocation, IN BOOL Operation);
extern _LONG_CALL_ int HalSPI0MCSPinCtrlRtl8195A(IN u32 PinLocation, IN BOOL Operation);
extern _LONG_CALL_ int HalI2C0PinCtrlRtl8195A(IN u32 PinLocation, IN BOOL Operation);
extern _LONG_CALL_ int HalI2C1PinCtrlRtl8195A(IN u32 PinLocation, IN BOOL Operation);
extern _LONG_CALL_ int HalI2C2PinCtrlRtl8195A(IN u32 PinLocation, IN BOOL Operation);
extern _LONG_CALL_ int HalI2C3PinCtrlRtl8195A(IN u32 PinLocation, IN BOOL Operation);
extern _LONG_CALL_ int HalI2S0PinCtrlRtl8195A(IN u32 PinLocation, IN BOOL Operation);
extern _LONG_CALL_ int HalI2S1PinCtrlRtl8195A(IN u32 PinLocation, IN BOOL Operation);
extern _LONG_CALL_ int HalPCM0PinCtrlRtl8195A(IN u32 PinLocation, IN BOOL Operation);
extern _LONG_CALL_ int HalPCM1PinCtrlRtl8195A(IN u32 PinLocation, IN BOOL Operation);
extern _LONG_CALL_ int HalSDIODPinCtrlRtl8195A(IN u32 PinLocation, IN BOOL Operation);
extern _LONG_CALL_ int HalSDIOHPinCtrlRtl8195A(IN u32 PinLocation, IN BOOL Operation);
extern _LONG_CALL_ int HalMIIPinCtrlRtl8195A(IN u32 PinLocation, IN BOOL Operation);
extern _LONG_CALL_ int HalWLLEDPinCtrlRtl8195A(IN u32 PinLocation, IN BOOL Operation);
extern _LONG_CALL_ int HalWLANT0PinCtrlRtl8195A(IN u32 PinLocation, IN BOOL Operation);
extern _LONG_CALL_ int HalWLANT1PinCtrlRtl8195A(IN u32 PinLocation, IN BOOL Operation);
extern _LONG_CALL_ int HalWLBTCOEXPinCtrlRtl8195A(IN u32 PinLocation, IN BOOL Operation);
extern _LONG_CALL_ int HalWLBTCMDPinCtrlRtl8195A(IN u32 PinLocation, IN BOOL Operation);
extern _LONG_CALL_ int HalNFCPinCtrlRtl8195A(IN u32 PinLocation, IN BOOL Operation);
extern _LONG_CALL_ int HalPWM0PinCtrlRtl8195A(IN u32 PinLocation, IN BOOL Operation);
extern _LONG_CALL_ int HalPWM1PinCtrlRtl8195A(IN u32 PinLocation, IN BOOL Operation);
extern _LONG_CALL_ int HalPWM2PinCtrlRtl8195A(IN u32 PinLocation, IN BOOL Operation);
extern _LONG_CALL_ int HalPWM3PinCtrlRtl8195A(IN u32 PinLocation, IN BOOL Operation);
extern _LONG_CALL_ int HalETE0PinCtrlRtl8195A(IN u32 PinLocation, IN BOOL Operation);
extern _LONG_CALL_ int HalETE1PinCtrlRtl8195A(IN u32 PinLocation, IN BOOL Operation);
extern _LONG_CALL_ int HalETE2PinCtrlRtl8195A(IN u32 PinLocation, IN BOOL Operation);
extern _LONG_CALL_ int HalETE3PinCtrlRtl8195A(IN u32 PinLocation, IN BOOL Operation);
extern _LONG_CALL_ int HalEGTIMPinCtrlRtl8195A(IN u32 PinLocation, IN BOOL Operation);
extern _LONG_CALL_ int HalSPIFlashPinCtrlRtl8195A(IN u32 PinLocation, IN BOOL Operation);
extern _LONG_CALL_ int HalSDRPinCtrlRtl8195A(IN u32 PinLocation, IN BOOL Operation);
extern _LONG_CALL_ int HalJTAGPinCtrlRtl8195A(IN u32 PinLocation, IN BOOL Operation);
extern _LONG_CALL_ int HalTRACEPinCtrlRtl8195A(IN u32 PinLocation, IN BOOL Operation);
extern _LONG_CALL_ int HalLOGUartPinCtrlRtl8195A(IN u32 PinLocation, IN BOOL Operation);
extern _LONG_CALL_ int HalLOGUartIRPinCtrlRtl8195A(IN u32 PinLocation, IN BOOL Operation);
extern _LONG_CALL_ int HalSICPinCtrlRtl8195A(IN u32 PinLocation, IN BOOL Operation);
extern _LONG_CALL_ int HalEEPROMPinCtrlRtl8195A(IN u32 PinLocation, IN BOOL Operation);
extern _LONG_CALL_ int HalDEBUGPinCtrlRtl8195A(IN u32 PinLocation, IN BOOL Operation);
#endif

//Location Index(Pin Mux Selection)
#define S0          0
#define S1          1
#define S2          2
#define S3          3

_LONG_CALL_ u8 HalPinCtrlRtl8195A(IN u32 Function, IN u32 PinLocation, IN BOOL Operation);

u8 GpioFunctionChk(IN u32 chip_pin, IN u8 Operation);
u8 FunctionChk(IN u32 Function, IN u32 PinLocation);

#endif // _HAL_PINMUX_
