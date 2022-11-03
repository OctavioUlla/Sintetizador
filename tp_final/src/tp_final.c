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
// Arreglo de las 13 notas inicializado en la 4ta Octava
uint16_t notas[13]= {262,277,294,311,330,349,370,392,415,440,466,494,523};
// Alpha para el filtrado del adc
uint16_t alpha = 1000;

Stack stack;

int main(void) {
 	SystemInit();

	stack = CreateStack();

	makeSignals(signals,actualSig);
	cfgPines();
	cfgTIM1();
	cfgDAC();
	cfgDMA(&actualSig[0]);

	cfgTIM0();
	cfgADC();
	cfgNVIC();
	/* TODO:	 *
	 * Configurar Interruciones donde se cambie segun la tecla el valor de la frecuencia de la señal mediante el DACCOUNTERVAL
	 * y empiece a pedir datos(HABILIAR LAS REQ DEL CANAL 0 del DMA) cuando se presiona un tecla, y que el DMA deje de recibir
	 * requests cuando se levanta la tecla
	 * Configurar el ADC para que cambie los valores del actualSig
*/

	while(1);

    return 0;
}

// Handler del cambio a señal previa
void EINT0_IRQHandler(void){
	TIM_Cmd(LPC_TIM1,ENABLE);
	while(TIM_GetIntStatus(LPC_TIM1,TIM_MR0_INT) != SET);
	TIM_ClearIntPending(LPC_TIM1,TIM_MR0_INT);

	prevSgn(&sgnActual,signals);
	EXTI_ClearEXTIFlag(EXTI_EINT0);
}

// Handler del cambio a señal siguiente
void EINT1_IRQHandler(void){
	TIM_Cmd(LPC_TIM1,ENABLE);
	while(TIM_GetIntStatus(LPC_TIM1,TIM_MR0_INT) != SET);
	TIM_ClearIntPending(LPC_TIM1,TIM_MR0_INT);

	nextSgn(&sgnActual,signals);
	EXTI_ClearEXTIFlag(EXTI_EINT1);
}

// Handler del cambio a octava superior
void EINT2_IRQHandler(void){
	TIM_Cmd(LPC_TIM1,ENABLE);
	while(TIM_GetIntStatus(LPC_TIM1,TIM_MR0_INT) != SET);
	TIM_ClearIntPending(LPC_TIM1,TIM_MR0_INT);

	aumentarOct(&octActual,notas);
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
			UpdateDMAFrecuency(&stack,notas);

			GPIO_ClearInt(0,(1<<i));
			EXTI_ClearEXTIFlag(EXTI_EINT3);
			return;
		}
		// Se ve si se aprieta una tecla
		else if(GPIO_GetIntStatus(0,i,1) == ENABLE){
			InsertTecla(&stack,i);
			UpdateDMAFrecuency(&stack,notas);

			GPIO_ClearInt(0,(1<<i));
			EXTI_ClearEXTIFlag(EXTI_EINT3);
			return;
		}
	}

	// Se ve si se toca la tecla para pin 15
	if(GPIO_GetIntStatus(0,15,0)){
		RemoveTecla(&stack,15);
		UpdateDMAFrecuency(&stack,notas);
		GPDMA_ChannelCmd(0,ENABLE);
		GPIO_ClearInt(0,1<<15);
		EXTI_ClearEXTIFlag(EXTI_EINT3);
		return;
	}
	// Se ve si se suelta la tecla para pin 15
	else if(GPIO_GetIntStatus(0,15,1)){
		InsertTecla(&stack,15);
		UpdateDMAFrecuency(&stack,notas);

		GPIO_ClearInt(0,1<<15);
		EXTI_ClearEXTIFlag(EXTI_EINT3);
		return;
	}

	disminuirOct(&octActual, notas);
	EXTI_ClearEXTIFlag(EXTI_EINT3);


}

// Handler del ADC
void ADC_IRQHandler(void){
	//uint16_t pePito = ADC_ChannelGetData(LPC_ADC,0);
	uint16_t prev_sgn = 0;
	uint16_t aux[13];
	static uint16_t cutoff = 0;
	static uint16_t pitch = 0;

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

		for(int i =0; i<14;i++){
			aux[i] = (uint16_t)(notas[i] + (notas[i]*(pitch-2048))/4097);
		}

		uint32_t dmaCounter =(uint32_t) (25 * 1000000)/(aux[GetNumTecla(&stack)]*TRANSFERSIZE);
		DAC_SetDMATimeOut(LPC_DAC,dmaCounter);

		ADC_ChannelCmd(LPC_ADC,1,DISABLE);
		ADC_ChannelCmd(LPC_ADC,0,ENABLE);
	}
}











