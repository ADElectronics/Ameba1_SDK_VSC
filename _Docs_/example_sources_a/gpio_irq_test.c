/*
 * test.c
 *
 *  Created on: 12 марта 2017 г.
 *      Author: PVV
 */

#include <platform_opts.h>
#include "device.h"
#include "gpio_api.h"   // mbed
#include "gpio_irq_api.h"   // mbed
#include "gpio_irq_ex_api.h"   // mbed
#include "timer_api.h"
#include "diag.h"
#include "main.h"

#include "hal_diag.h"
#include "rtl8195a/rtl_libc.h"

#define GPIO_LED_PIN        PA_4
#define GPIO_IRQ_PIN        PC_4

gpio_irq_t gpio_btn;
gpio_t gpio_led;
gtimer_t my_timer;

uint32_t lo_time_us, hi_time_us;
uint32_t lo_time_cnt, hi_time_cnt;
uint32_t old_tsf;

uint32_t io_irq_count;

//-- Test tsf (64-bits counts, 1 us step) ---

//#include "hal_com_reg.h"
#define WIFI_REG_BASE               0x40080000
#define REG_TSFTR						0x0560
#define REG_TSFTR1						0x0568	// HW Port 1 TSF Register

#define ReadTSF_Lo32() (*((volatile unsigned int *)(WIFI_REG_BASE + REG_TSFTR)))
#define ReadTSF_Hi32() (*((volatile unsigned int *)(WIFI_REG_BASE + REG_TSFTR1)))
// WiFi должен быть включен!
LOCAL uint64_t get_tsf(void)
{
	return *((uint64_t *)(WIFI_REG_BASE + REG_TSFTR));
}

LOCAL void gpio_demo_irq_handler(uint32_t id, gpio_irq_event event) {

//    gpio_irq_disable(&gpio_btn);
	io_irq_count++;
	uint32_t new_tsf = get_tsf();
	uint32_t delta_us = (uint32_t) new_tsf - (uint32_t) old_tsf;
	if (event & 1) {
		lo_time_us += delta_us;
		lo_time_cnt++;
		gpio_irq_set(&gpio_btn, IRQ_LOW, 1);
	} else {
		hi_time_us += delta_us;
		hi_time_cnt++;
		gpio_irq_set(&gpio_btn, IRQ_HIGH, 1);
	}
	old_tsf = new_tsf;
//	gpio_irq_enable(&gpio_btn);
}

LOCAL void timer1_timeout_handler(uint32_t id) {
	uint32_t lo, hi, ti, fi;

	if (lo_time_cnt && hi_time_cnt) {
		lo = lo_time_us / lo_time_cnt;
		hi = hi_time_us / hi_time_cnt;
		ti = hi + lo;
		fi = 0;
		if(ti) fi = 1000000/ti;
		lo_time_cnt = 0;
		lo_time_us = 0;
		hi_time_cnt = 0;
		hi_time_us = 0;
		printf("Period: %lu us (%lu hz), Lo: %lu us, Hi: %lu us\n", ti, fi,  lo, hi);
	}
}

/**
 * @brief  Main program.
 * @param  None
 * @retval None
 */
LOCAL char ipc4_init;
LOCAL void fATIPC4(int argc, char *argv[]) {

	lo_time_cnt = 0;
	lo_time_us = 0;
	hi_time_cnt = 0;
	hi_time_us = 0;
	if(!ipc4_init) {
		rtl_printf("Init test...\n");
		// Init LED control pin
		gpio_init(&gpio_led, GPIO_LED_PIN);
		gpio_dir(&gpio_led, PIN_OUTPUT);    // Direction: Output
		gpio_mode(&gpio_led, PullNone);     // No pull
		gpio_write(&gpio_led, 0);

		// Initial Push Button pin as interrupt source
		gpio_irq_init(&gpio_btn, GPIO_IRQ_PIN, gpio_demo_irq_handler,
				(uint32_t) (&gpio_led));
		gpio_irq_set(&gpio_btn, IRQ_FALL, 1);   // Falling Edge Trigger
		gpio_irq_enable(&gpio_btn);

		// Initial a periodical timer
		gtimer_init(&my_timer, TIMER1);
		gtimer_start_periodical(&my_timer, 1000000, (void*) timer1_timeout_handler,
				(uint32_t) &gpio_led);
		ipc4_init = 1;
	}
	else {
		rtl_printf("DeInit test.\n");
		gtimer_deinit(&my_timer);
		gpio_irq_deinit(&gpio_btn);
		gpio_dir(&gpio_led, PIN_INPUT);
		ipc4_init = 0;
	}
}

MON_RAM_TAB_SECTION COMMAND_TABLE console_commands_irqtest[] = {
		{ "ATIPC4", 0, fATIPC4, ":  Test input pulse in PC_4" } };

