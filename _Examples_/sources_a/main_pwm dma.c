/*
 * Test DMA PWM 
 */

#include "ameba_soc.h"
#include "main.h"

#define PWM_PERIOD	1000 

GDMA_InitTypeDef GdmaInitStruct;

u32 * PWM_pattern = 0;

int pwm_chan = 0;

GDMA_InitTypeDef TIMGdmaInitStruct;

void tim5_gen_pwm_dma_ISR(void *Data)
{
}

void tim5_gen_pwm_dma()
{
    
	RTIM_TimeBaseInitTypeDef TIM_InitStruct_temp;
	TIM_CCInitTypeDef TIM_CCInitStruct;
	
	u32 * PWM_pattern = pvPortMalloc(PWM_PERIOD*4);
	if (!PWM_pattern) { 
		DBG_8195A("Not Memory!\n\r");
		return;
	}
	int i;
	for (i=0; i< PWM_PERIOD/2; i++) {
		PWM_pattern[i] = i + 0x3000000;
		PWM_pattern[PWM_PERIOD/2 + i] = PWM_PERIOD/2 - i + 0x3000000;
	}

	RTIM_TimeBaseStructInit(&TIM_InitStruct_temp);
	TIM_InitStruct_temp.TIM_Idx = 5;
	TIM_InitStruct_temp.TIM_Prescaler = 0; 
	TIM_InitStruct_temp.TIM_Period = PWM_PERIOD/2 - 1;
	RTIM_TimeBaseInit(TIM5, &TIM_InitStruct_temp, TIMER5_IRQ, NULL, 0);

	RTIM_CCStructInit(&TIM_CCInitStruct);
	RTIM_CCxInit(TIM5, &TIM_CCInitStruct, pwm_chan);
	RTIM_CCRxSet(TIM5, 1, pwm_chan);
	RTIM_CCxCmd(TIM5, pwm_chan, TIM_CCx_Enable);

	Pinmux_Config(_PA_23, PINMUX_FUNCTION_PWM);
	RTIM_Cmd(TIM5, ENABLE);

	/* enable pwm channel DMA*/
	InterruptDis(TIMER5_IRQ);
	RTIM_DMACmd(TIM5, TIM_DMA_CCx[pwm_chan], ENABLE);
	RTIM_TXGDMA_Init(pwm_chan, &TIMGdmaInitStruct, NULL, NULL, (u8*)PWM_pattern, PWM_PERIOD*4);
}


void task_pwm_dma(void)
{
	/* generate breathing light pwm wave*/
	tim5_gen_pwm_dma();

	vTaskDelete(NULL);
}

/**
  * @brief  Main program.
  * @param  None
  * @retval None
  */
void main(void)
{
	if(xTaskCreate( (TaskFunction_t)task_pwm_dma, "TCAPDMA", (2048/4), NULL, (tskIDLE_PRIORITY + 1), NULL)!= pdPASS) {
			DBG_8195A("Cannot create tim4 capture dma demo task\n\r");
	}

	vTaskStartScheduler();

	while(1);
}


