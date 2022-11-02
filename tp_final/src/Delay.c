#include "Delay.h"
#include "lpc17xx_timer.h"

void Delay(uint32_t ms){
	/*
	 TIM_MATCHCFG_Type match;
	 match.MatchChannel = 0;
	 match.IntOnMatch = ENABLE;
	 match.StopOnMatch = ENABLE;
	 match.ResetOnMatch = ENABLE;
	 match.ExtMatchOutputType =  TIM_EXTMATCH_NOTHING;
	 match.MatchValue = ms*10;

	 TIM_ConfigMatch(LPC_TIM2, &match);

	 TIM_Cmd(LPC_TIM2,ENABLE);
	 while(TIM_GetIntStatus(LPC_TIM2,TIM_MR0_INT) != SET);
	 TIM_ClearIntPending(LPC_TIM2,TIM_MR0_INT);
	 */

	TIM_Cmd(LPC_TIM1,ENABLE);
	while(TIM_GetIntStatus(LPC_TIM1,TIM_MR0_INT) != SET);
	TIM_ClearIntPending(LPC_TIM1,TIM_MR0_INT);
}

