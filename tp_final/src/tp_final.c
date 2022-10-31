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
#include <cr_section_macros.h>

#include <stdio.h>
#include <Teclas.h>

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
uint8_t sgnActual = 0;
uint8_t octActual = 4;
// Arreglo de las 13 notas inicializado en la 4ta Octava
uint16_t notas[13]= {262,277,294,311,330,349,370,392,415,440,466,494,523};
// Lista a la que apunta el dma, se crea global para variar la senial


Stack stack;

int main(void) {
	SystemInit();

	stack = CreateStack();

	makeSignals(signals,actualSig);
	cfgPines();
	cfgDAC();
	cfgDMA(&actualSig[0]);

	// Se desactiva la interrupcion para que el dac no comience a pedir datos

	//25MHz por clock del CPU 100MHz y transferSize

	//cfgTIM0();
	//cfgADC();
	cfgNVIC();
	/* TODO:	 *
	 * Configurar Interruciones donde se cambie segun la tecla el valor de la frecuencia de la señal mediante el DACCOUNTERVAL
	 * y empiece a pedir datos(HABILIAR LAS REQ DEL CANAL 0 del DMA) cuando se presiona un tecla, y que el DMA deje de recibir
	 * requests cuando se levanta la tecla
	 * Configurar el ADC para que cambie los valores de
*/
	while(1);
    return 0 ;
}

void EINT0_IRQHandler(void){
	prevSgn(&sgnActual,signals);
}

void EINT1_IRQHandler(void){
	nextSgn(&sgnActual,signals);
}

void EINT2_IRQHandler(void){
	aumentarOct(&octActual,notas);

}

void EINT3_IRQHandler(void){

	//Pines del 0 al 11
	for(uint8_t i = 0;i<12;i++){
		//Ver si se solto la tecla
		if(GPIO_GetIntStatus(0,i,1) == ENABLE){
			InsertTecla(&stack,i);
			UpdateDMAFrecuency(&stack,notas);

			GPIO_ClearInt(0,i);
			return;
		}
		//Ver si se apreto la tecla
		else if(GPIO_GetIntStatus(0,i,0) == ENABLE){
			RemoveTecla(&stack,i);
			UpdateDMAFrecuency(&stack,notas);

			GPIO_ClearInt(0,i);
			return;
		}
	}

	//Ver si se apreto la tecla para pin 15
	if(GPIO_GetIntStatus(0,15,0)){
		RemoveTecla(&stack,15);
		UpdateDMAFrecuency(&stack,notas);

		GPIO_ClearInt(0,15);
		return;
	}
	//Ver si se apreto la tecla para pin 15
	else if(GPIO_GetIntStatus(0,15,1)){
		InsertTecla(&stack,15);
		UpdateDMAFrecuency(&stack,notas);

		GPIO_ClearInt(0,15);
		return;
	}

	disminuirOct(&octActual, notas);


}

void ADC_IRQHandler(){
	//TODO esto
}











