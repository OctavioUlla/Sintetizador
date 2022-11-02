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
uint8_t sgnActual = 1;
uint8_t octActual = 4;
// Arreglo de las 13 notas inicializado en la 4ta Octava
uint16_t notas[13]= {262,277,294,311,330,349,370,392,415,440,466,494,523};
// Alpha para el filtrado del adc
uint16_t alpha = 420;

Stack stack;

int main(void) {
	SystemInit();

	stack = CreateStack();

	makeSignals(signals,actualSig);
	cfgPines();
	cfgTIM1();
	cfgTIM2();
	cfgDAC();
	cfgDMA(&actualSig[0]);

	cfgI2C();

	//cfgTIM0();
	//cfgADC();
	cfgNVIC();
	/* TODO:	 *
	 * Configurar Interruciones donde se cambie segun la tecla el valor de la frecuencia de la señal mediante el DACCOUNTERVAL
	 * y empiece a pedir datos(HABILIAR LAS REQ DEL CANAL 0 del DMA) cuando se presiona un tecla, y que el DMA deje de recibir
	 * requests cuando se levanta la tecla
	 * Configurar el ADC para que cambie los valores del actualSig
*/



	while(1){

	};
    return 0 ;
}

// Handler del cambio a señal previa
void EINT0_IRQHandler(void){
	prevSgn(&sgnActual,signals);
	EXTI_ClearEXTIFlag(EXTI_EINT0);
}

// Handler del cambio a señal siguiente
void EINT1_IRQHandler(void){
	nextSgn(&sgnActual,signals);
	EXTI_ClearEXTIFlag(EXTI_EINT1);
}

// Handler del cambio a octava superior
void EINT2_IRQHandler(void){
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
void ADC_IRQHandler(){
	static int16_t prev_cutoff = 0;
	static int16_t prev_pitch = 0;
	static int16_t prev_sgn = 0;
	int16_t cutoff = (int16_t)(((ADC_ChannelGetData(LPC_ADC,0)*alpha)/4097) + ((prev_cutoff*(4097-alpha))/4097));
	int16_t pitch = (int16_t)(((ADC_ChannelGetData(LPC_ADC,1)*alpha)/4097) + ((prev_pitch*(4097-alpha))/4097));
	if((cutoff-prev_cutoff) > 100 || (cutoff-prev_cutoff) < -100){
		for(int i =0;i<TRANSFERSIZE;i++){
			actualSig[i] = (int16_t)(((signals[sgnActual][i]*cutoff)/4097) + ((prev_sgn*(4097-cutoff))/4097));
			prev_sgn = actualSig[i];
		}
	}
	if((pitch-prev_pitch)>100 || (pitch-prev_pitch)< -100){
		for(int i =0; i<14;i++){
			notas[i] = (uint16_t)((notas[i]*pitch)/4097);
		}
		uint32_t dmaCounter = (25 * 1000000)/(notas[GetNumTecla(&stack)]*TRANSFERSIZE);
		DAC_SetDMATimeOut(LPC_DAC,dmaCounter);
	}

}











