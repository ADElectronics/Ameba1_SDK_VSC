#ifndef UART_UPDATE_H
#define UART_UPDATE_H

#include "PinNames.h"
#include "gpio_api.h"
//#include "hal_gpio.h"
//#include "osdep_api.h"
#include "osdep_service.h"

#define MAX_WAIT_TIME	100

#if defined(CONFIG_PLATFORM_8711B)
#define PIN_NAME		PA_5
#else
#define PIN_NAME		PC_2
#endif

void example_uart_update();
int is_update_image_enable(gpio_t *gpio_uart_update_eable);
extern int uart_ymodem(void);

#endif
