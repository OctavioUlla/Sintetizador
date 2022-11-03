/*
===============================================================================
 Name        : tp_final.c
 Author      : Los Sintetizadores
 Version     : 0.0.1
 Copyright   : $(copyright)
 Description : main definition
===============================================================================
*/

#include "lpc17xx_dac.h"
#include "lpc17xx_timer.h"
#include "lpc17xx_pinsel.h"
#include "lpc17xx_gpio.h"
#include "lpc17xx_exti.h"
#include "lpc17xx_gpdma.h"
#include "lpc17xx_systick.h"
#include <cr_section_macros.h>

#include <stdio.h>

#include "Teclas.h"
#include "Config.h"
#include "Botones.h"
#include "Display.h"
#include "Delay.h"

// TODO: insert other definitions and declarations here



// Arreglo de arreglos donde se guardan las formas de onda
// Posicion  de las seniales en el enum sgnls en config.h
uint32_t signals[CANTIDADSGNLS][TRANSFERSIZE];
uint32_t actualSig[TRANSFERSIZE];
//uint16_t sgnRect[TRANSFERSIZE]; // 0
//uint16_t sgnTriang[TRANSFERSIZE]; // 1
//uint16_t sgnSierra[TRANSFERSIZE]; // 2

// Entero para seleccionar la señal
uint8_t sgnActual = 3;
uint8_t octActual = 4;
uint16_t aux[13];
// Arreglo de las 13 notas inicializado en la 4ta Octava
uint16_t notas[8][13]= {{16,17,18,19,21,22,23,24,26,27,29,31,33}, // Octava 0
						{33,35,37,39,41,44,46,49,52,55,58,62,65}, // Octava 1
						{65,69,73,78,82,87,92,98,104,110,117,123,131}, // Octava 2
						{131,139,147,156,167,175,185,196,208,220,233,247,262}, // Octava 3
						{262,277,294,311,330,349,370,392,415,440,466,494,523}, // Octava 4
						{523,554,587,622,659,698,740,784,831,880,932,988,1046}, // Octava 5
						{1046,1109,1175,1245,1319,1397,1480,1568,1661,1760,1865,1976,2093}, // Octava 6
						{2093,2218,2349,2489,2637,2794,2960,3136,3322,3520,3729,3900,3900}}; // Octava 7
// Alpha para el filtrado del adc
uint16_t alpha = 1000;
int8_t tecla = -1;

Stack stack;

int main(void) {
 	SystemInit();

	stack = CreateStack();

	makeSignals(signals,actualSig);
	cfgPines();
	cfgTIM1();
	cfgTIM2();
	cfgDAC();
	cfgDMA(actualSig);

	cfgI2C();
  
	cfgTIM0();
	cfgADC();
  
	cfgNVIC();

	while(1){
		Delay(20);
		if(tecla == -1){
			ShowData(0,sgnActual);
		}
		else ShowData(aux[tecla],sgnActual);
	}

  return 0;
}

// Handler del cambio a señal previa
void EINT0_IRQHandler(void){
	TIM_Cmd(LPC_TIM1,ENABLE);
	while(TIM_GetIntStatus(LPC_TIM1,TIM_MR0_INT) != SET);
	TIM_ClearIntPending(LPC_TIM1,TIM_MR0_INT);

	prevSgn(&sgnActual);
	EXTI_ClearEXTIFlag(EXTI_EINT0);
}

// Handler del cambio a señal siguiente
void EINT1_IRQHandler(void){
	TIM_Cmd(LPC_TIM1,ENABLE);
	while(TIM_GetIntStatus(LPC_TIM1,TIM_MR0_INT) != SET);
	TIM_ClearIntPending(LPC_TIM1,TIM_MR0_INT);

	nextSgn(&sgnActual);
	EXTI_ClearEXTIFlag(EXTI_EINT1);
}

// Handler del cambio a octava superior
void EINT2_IRQHandler(void){
	TIM_Cmd(LPC_TIM1,ENABLE);
	while(TIM_GetIntStatus(LPC_TIM1,TIM_MR0_INT) != SET);
	TIM_ClearIntPending(LPC_TIM1,TIM_MR0_INT);

	aumentarOct(&octActual);
	EXTI_ClearEXTIFlag(EXTI_EINT2);

}

// Handler del cambio a octava superior y las otras teclas del teclado
void EINT3_IRQHandler(void){
	// Deboucing
	TIM_Cmd(LPC_TIM1,ENABLE);
	while(TIM_GetIntStatus(LPC_TIM1,TIM_MR0_INT) != SET);
	TIM_ClearIntPending(LPC_TIM1,TIM_MR0_INT);

	//Pines del 0 al 11
	for(uint8_t i = 0;i<12;i++){
		// Se ve si se suelta una tecla
		if(GPIO_GetIntStatus(0,i,0) == ENABLE){
			RemoveTecla(&stack,i);
			tecla = GetNumTecla(&stack);
			UpdateDMAFrecuency(&stack,notas[octActual]);
			GPIO_ClearInt(0,(1<<i));
			EXTI_ClearEXTIFlag(EXTI_EINT3);
			return;
		}
		// Se ve si se aprieta una tecla
		else if(GPIO_GetIntStatus(0,i,1) == ENABLE){
			InsertTecla(&stack,i);
			tecla = GetNumTecla(&stack);
			UpdateDMAFrecuency(&stack,notas[octActual]);
			GPIO_ClearInt(0,(1<<i));
			EXTI_ClearEXTIFlag(EXTI_EINT3);
			return;
		}
	}

	// Se ve si se suelta la tecla para pin 15
	if(GPIO_GetIntStatus(0,15,0)){
		RemoveTecla(&stack,12);
		tecla = GetNumTecla(&stack);
		UpdateDMAFrecuency(&stack,notas[octActual]);
		GPIO_ClearInt(0,1<<15);
		EXTI_ClearEXTIFlag(EXTI_EINT3);
		return;
	}
	// Se ve si se toca la tecla para pin 15
	else if(GPIO_GetIntStatus(0,15,1)){
		InsertTecla(&stack,12);
		tecla = GetNumTecla(&stack);
		UpdateDMAFrecuency(&stack,notas[octActual]);
		GPIO_ClearInt(0,1<<15);
		EXTI_ClearEXTIFlag(EXTI_EINT3);
		return;
	}

	disminuirOct(&octActual);
	EXTI_ClearEXTIFlag(EXTI_EINT3);


}

// Handler del ADC
void ADC_IRQHandler(void){
	//uint16_t pePito = ADC_ChannelGetData(LPC_ADC,0);
	static uint16_t cutoff = 0;
	static uint16_t pitch = 0;
	uint16_t prev_sgn = 0;

	if(ADC_ChannelGetStatus(LPC_ADC,0,ADC_DATA_DONE)){

		cutoff = (uint16_t)(((ADC_ChannelGetData(LPC_ADC,0)*alpha)/4095) + ((cutoff*(4095-alpha))/4095));

		if(cutoff > 5){
			for(int i =0;i<TRANSFERSIZE;i++){
				actualSig[i] = (uint16_t)(((signals[sgnActual][i]*cutoff)/4095) + ((prev_sgn*(4095-cutoff))/4095));
				prev_sgn = actualSig[i];
			}
		}

		ADC_ChannelCmd(LPC_ADC,0,DISABLE);
		ADC_ChannelCmd(LPC_ADC,1,ENABLE);
	}
	else if(ADC_ChannelGetStatus(LPC_ADC,1,ADC_DATA_DONE)){

		pitch = (int16_t)(((ADC_ChannelGetData(LPC_ADC,1)*alpha)/4095) + ((pitch*(4095-alpha))/4095));

		for(int i =0; i<13;i++){
			aux[i] = (uint16_t)((notas[octActual][i]) + ((notas[octActual][i]*(pitch-2048))/8195));
		}

		uint32_t dmaCounter =(uint32_t) (25 * 1000000)/(aux[tecla]*TRANSFERSIZE);
		DAC_SetDMATimeOut(LPC_DAC,dmaCounter);

		ADC_ChannelCmd(LPC_ADC,1,DISABLE);
		ADC_ChannelCmd(LPC_ADC,0,ENABLE);
	}


}











