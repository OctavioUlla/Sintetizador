#include "Delay.h"
#include "lpc17xx_timer.h"

void Delay(uint32_t ms){
	 TIM_UpdateMatchValue(LPC_TIM2,0, ms);

	 TIM_Cmd(LPC_TIM2,ENABLE);
	 while(TIM_GetIntStatus(LPC_TIM2,TIM_MR0_INT) != SET);
	 TIM_ClearIntPending(LPC_TIM2,TIM_MR0_INT);

	 TIM_ResetCounter(LPC_TIM2);
}

