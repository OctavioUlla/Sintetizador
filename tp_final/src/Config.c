/*
 * config.c
 *
 *  Created on: 28 oct. 2022
 *      Author: ferminverdolini
 */

#include "Config.h"
#include "Delay.h"
#include "Display.h"

// Funcion que configura los puertos
void cfgPines(){

	// Se configuran los pines y las interrupciones de las 13 teclas del piano
	PINSEL_CFG_Type pincfg;
	pincfg.Portnum = 0; // Puerto 0
	pincfg.Funcnum = 0; // Funcion GPIO
	pincfg.OpenDrain = PINSEL_PINMODE_NORMAL;
	pincfg.Pinmode = PINSEL_PINMODE_PULLUP; // Resistencias de pull up
	for(int i = 0; i<12; i++){
		pincfg.Pinnum = i;
		PINSEL_ConfigPin(&pincfg); // Se configuran los pines del 0 al 11

	}
	pincfg.Pinnum = 15; // Se configura la 13ra tecla, puerto 15
	PINSEL_ConfigPin(&pincfg);
	GPIO_ClearInt(0,(0xFFF | (1<<15)));
	GPIO_IntCmd(0,(0xFFF | (1<<15)),0); // Se habilitan interrupciones por flanco de subida y de bajada
	GPIO_IntCmd(0,(0xFFF | (1<<15)),1);


	// Se configuran las teclas de control de ondas y de cambio de octavas
	pincfg.Portnum = 2; // Puerto 2
	pincfg.Pinnum = 1; // Funcion EINT
	pincfg.OpenDrain = PINSEL_PINMODE_NORMAL;
	pincfg.Pinmode = PINSEL_PINMODE_PULLUP; // Resistencias de pull up
	for(int i = 10;i<14; i++){
		pincfg.Pinnum = i;
		PINSEL_ConfigPin(&pincfg);
	}
	// Se configuran las interrupciones externas TODO(ver de cambiar por manejo de bytes)
	EXTI_InitTypeDef extcfg;
	extcfg.EXTI_Mode = EXTI_MODE_EDGE_SENSITIVE;
	extcfg.EXTI_polarity = EXTI_POLARITY_LOW_ACTIVE_OR_FALLING_EDGE;
	extcfg.EXTI_Line = EXTI_EINT0;
	EXTI_Config(&extcfg);
	EXTI_ClearEXTIFlag(EXTI_EINT0);
	extcfg.EXTI_Line = EXTI_EINT1;
	EXTI_Config(&extcfg);
	EXTI_ClearEXTIFlag(EXTI_EINT1);
	extcfg.EXTI_Line = EXTI_EINT2;
	EXTI_Config(&extcfg);
	EXTI_ClearEXTIFlag(EXTI_EINT2);
	extcfg.EXTI_Line = EXTI_EINT3;
	EXTI_Config(&extcfg);
	EXTI_ClearEXTIFlag(EXTI_EINT3);


}

// Funcion que configura el DMA
void cfgDAC(){

	// Se configura el puerto 0.26 como AOUT
	PINSEL_CFG_Type dac;
	dac.Portnum = 0;
	dac.Pinnum = 26;
	dac.Pinmode = 0;
	dac.Funcnum = 2;
	dac.OpenDrain = PINSEL_PINMODE_NORMAL;
	PINSEL_ConfigPin(&dac); // Se prende el DAC
	// Se configura el DAC
	DAC_CONVERTER_CFG_Type daccfg;
	daccfg.CNT_ENA = RESET; // Se habilita el timeout counter
	daccfg.DMA_ENA = SET; // Se habilita las request a DMA
	DAC_Init(LPC_DAC);
	DAC_ConfigDAConverterControl(LPC_DAC,&daccfg);
}

// Funcion que configura el DMA y prende el DMA
void cfgDMA(uint32_t* actualSig){
	static GPDMA_LLI_Type listaDma;
	NVIC_DisableIRQ(DMA_IRQn);


	GPDMA_Init();
	// Se inicializa el DMA apuntando a la lista de la señal rectangula
	listaDma.NextLLI = (uint32_t) &listaDma;
	listaDma.SrcAddr = (uint32_t) actualSig;
	listaDma.DstAddr = (uint32_t) &(LPC_DAC ->DACR);
	listaDma.Control = (TRANSFERSIZE | (2 << 18) | ( 2 << 21)| (1 << 26));

	GPDMA_Channel_CFG_Type dmacfg;
	dmacfg.ChannelNum = 0;
	dmacfg.DstMemAddr = 0;
	dmacfg.TransferSize = TRANSFERSIZE;
	dmacfg.TransferWidth = 0;
	dmacfg.TransferType = GPDMA_TRANSFERTYPE_M2P;
	dmacfg.SrcMemAddr = (uint32_t) listaDma.SrcAddr;
	dmacfg.SrcConn = 0;
	dmacfg.DstConn = GPDMA_CONN_DAC;
	dmacfg.DMALLI = (uint32_t) &listaDma;

	GPDMA_Setup(&dmacfg);
	GPDMA_ChannelCmd(0,ENABLE);


}

//Funcion que configura el ADC
void cfgADC(){
	// Se configuran los puertos 0.23 y 0.24 como ADC0.0 y ADC0.1
	PINSEL_CFG_Type adc;
	adc.Portnum = 0;
	adc.OpenDrain = PINSEL_PINMODE_NORMAL;
	adc.Pinmode = 1;
	adc.Pinnum = 23;
	PINSEL_ConfigPin(&adc);
	adc.Pinnum = 24;
	PINSEL_ConfigPin(&adc);
	//  Se configura el ADC
	ADC_ChannelCmd(LPC_ADC,0,ENABLE); // Canal par el filtro cutoff
	ADC_ChannelCmd(LPC_ADC,1,ENABLE); // Canal par el pitch
	ADC_StartCmd(LPC_ADC,ADC_START_ON_MAT01);
	ADC_EdgeStartConfig(LPC_ADC,ADC_START_ON_RISING);
	ADC_IntConfig(LPC_ADC,ADC_ADGINTEN,ENABLE);

}

//Funcion que confidura el Timer0
void cfgTIM0(){
	TIM_TIMERCFG_Type timcfg;
	timcfg.PrescaleOption = TIM_PRESCALE_TICKVAL;
	timcfg.PrescaleValue = 250;
	// El timer cuenta cada (CCLCK/4)/ 250 = 100kHz = 10uS
	TIM_Init(LPC_TIM0,TIM_TIMER_MODE, &timcfg);

	 TIM_MATCHCFG_Type match;
	 match.MatchChannel = 1;
	 match.IntOnMatch = DISABLE;
	 match.StopOnMatch = DISABLE;
	 match.ResetOnMatch = ENABLE;
	 match.ExtMatchOutputType =  TIM_EXTMATCH_TOGGLE;
	 match.MatchValue = 50;
	 // El match sucede cada 50 clock del timer = 0.5mS
	 TIM_ConfigMatch(LPC_TIM0, & match);
	 TIM_Cmd(LPC_TIM0,ENABLE);
}

void cfgTIM1(){
	TIM_TIMERCFG_Type timcfg;
	timcfg.PrescaleOption = TIM_PRESCALE_USVAL;
	timcfg.PrescaleValue = 1000;
	// Prescaler en 1ms
	TIM_Init(LPC_TIM1,TIM_TIMER_MODE, &timcfg);

	 TIM_MATCHCFG_Type match;
	 match.MatchChannel = 0;
	 match.IntOnMatch = ENABLE;
	 match.StopOnMatch = ENABLE;
	 match.ResetOnMatch = ENABLE;
	 match.ExtMatchOutputType =  TIM_EXTMATCH_NOTHING;
	 match.MatchValue = 50;
	 // 100 ms
	 TIM_ConfigMatch(LPC_TIM1, &match);
}

void cfgTIM2(){
	TIM_TIMERCFG_Type timcfg;
	timcfg.PrescaleOption = TIM_PRESCALE_USVAL;
	timcfg.PrescaleValue = 100;
	// Prescaler en 0.1ms
	TIM_Init(LPC_TIM2,TIM_TIMER_MODE, &timcfg);
}

void cfgI2C(){
	DisplayInit(MODE_400kbps, 3);

	// 4 bit Init
	Delay(50);  // Esperar >40ms
	SendCmd(0x30);
	Delay(5);  // Esperar >4.1ms
	SendCmd(0x30);
	Delay(1);  // Esperar >100us
	SendCmd(0x30);
	Delay(10);
	SendCmd(0x20);  // Modo 4bit
	Delay(10);

	// Display Init
	SendCmd(0x28); // Function set --> DL=0 (4 bit mode), N = 1 (2 line display) F = 0 (5x8 characters)
	Delay(1);
	SendCmd(0x08); //Display on/off control --> D=0,C=0, B=0  ---> display off
	Delay(1);
	SendCmd(0x01);  // clear display
	Delay(1);
	Delay(1);
	SendCmd(0x06); //Entry mode set --> I/D = 1 (increment cursor) & S = 0 (no shift)
	Delay(1);
	SendCmd(0x0F); //Display on/off control --> D = 1, C = 1 and B = 0.
}


//Funcion que configura el NVIC
void cfgNVIC(){
	// Se definden las prioridades
	NVIC_SetPriority(EINT0_IRQn,1);
	NVIC_SetPriority(EINT1_IRQn,2);
	NVIC_SetPriority(EINT2_IRQn,3);
	NVIC_SetPriority(EINT3_IRQn,4);
	NVIC_SetPriority(ADC_IRQn,5);
	// Se habilitan las interrupciones
	NVIC_ClearPendingIRQ(EINT0_IRQn);
	NVIC_EnableIRQ(EINT0_IRQn);
	NVIC_ClearPendingIRQ(EINT1_IRQn);
	NVIC_EnableIRQ(EINT1_IRQn);
	NVIC_ClearPendingIRQ(EINT2_IRQn);
	NVIC_EnableIRQ(EINT2_IRQn);
	NVIC_ClearPendingIRQ(EINT3_IRQn);
	NVIC_EnableIRQ(EINT3_IRQn);
	NVIC_ClearPendingIRQ(ADC_IRQn);
	NVIC_EnableIRQ(ADC_IRQn);

}

//Funcion que crea las señales basicas
void makeSignals(uint32_t signals[][TRANSFERSIZE],uint32_t actualSig[TRANSFERSIZE]){
	uint16_t i;
	uint16_t pendiente = DACSIZE/TRANSFERSIZE;

	for(i=0;i<TRANSFERSIZE;i++){
		if(i<TRANSFERSIZE/2){
			signals[SGNRECT][i]=(0<<6);
			actualSig[i]=((i*pendiente*2)<<6);
			signals[SGNTRIANG][i]=((i*pendiente*2)<<6);
		}else{
			signals[SGNRECT][i]= ((DACSIZE-1) <<6);
			actualSig[i]= (((TRANSFERSIZE-i-1)*pendiente*2)<< 6);
			signals[SGNTRIANG][i]=(((TRANSFERSIZE-i-1)*pendiente*2)<< 6);
		}
		signals[SGNSIERRA][i]=((i*pendiente)<<6);
	}
}
