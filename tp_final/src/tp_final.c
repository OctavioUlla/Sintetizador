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

#include "Config.h"

// TODO: insert other definitions and declarations here

#define CANTIDADSGNLS  ((3))

// Arreglos donde se guardan las formas de onda
uint16_t sgnRect[TRANSFERSIZE]; // 0
uint16_t sgnTriang[TRANSFERSIZE]; // 1
uint16_t sgnSierra[TRANSFERSIZE]; // 2
// Entero para seleccionar la señal
uint8_t sgnActual = 0;
uint8_t octActual = 4;
// Arreglo de las 13 notas inicializado en la 4ta Octava
uint16_t notas[13]= {262,277,294,311,330,349,370,392,415,440,466,494,523};

int main(void) {
	makeSignals(sgnRect, sgnTriang, sgnSierra);
	cfgPines();
	cfgDAC();
	cfgDMA(sgnRect);

	/* TODO:
	 *
	 * Configurar NVIC pata las inteerrupciones externas
	 *
	 * Configurar Interruciones donde se cambie segun la tecla el valor de la frecuencia de la señal mediante el DACCOUNTERVAL
	 * y empiece a pedir datos(HABILIAR LAS REQ DEL CANAL 0 del DMA) cuando se presiona un tecla, y que el DMA deje de recibir
	 * requests cuando se levanta la tecla
*/
	while(1);
    return 0 ;
}

// Funcion que configura los puertos
void cfgPines(){

	// Se configuran los pines y las interrupciones de las 13 teclas del piano
	PINSEL_CFG_Type pincfg;
	pincfg.Portnum = 0; // Puerto 0
	pincfg.Funcnum = 0; // Funcion GPIO
	pincfg.Pinmode = PINSEL_PINMODE_PULLUP; // Resistencias de pull up
	for(int i = 0; i<12; i++){
		pincfg.Pinnum = i;
		PINSEL_ConfigPin(&pincfg); // Se configuran los pines del 0 al 11
	}
	pincfg.Pinnum = 15; // Se configura la 13ra tecla, puerto 15
	PINSEL_ConfigPin(&pincfg);
	GPIO_IntCmd(0,(0xFF || (1<<15)),0); // Se habilitan interrupciones por flanco de subida y de bajada
	GPIO_IntCmd(0,(0xFF || (1<<15)),1);

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
	extcfg.EXTI_Mode = EXTI_MODE_LEVEL_SENSITIVE;
	extcfg.EXTI_polarity = EXTI_POLARITY_LOW_ACTIVE_OR_FALLING_EDGE;
	extcfg.EXTI_Line = EXTI_EINT0;
	EXTI_Config(&extcfg);
	extcfg.EXTI_Line = EXTI_EINT1;
	EXTI_Config(&extcfg);
	extcfg.EXTI_Line = EXTI_EINT2;
	EXTI_Config(&extcfg);
	extcfg.EXTI_Line = EXTI_EINT3;
	EXTI_Config(&extcfg);

}



void EINT0_IRQHandler(void){
	sgnActual--;
	if(sgnActual<0){
		sgnActual=CANTIDADSGNLS-1;
	}
}

void EINT1_IRQHandler(void){
	sgnActual++;
	if(sgnActual==CANTIDADSGNLS){
		sgnActual=0;
	}
}

void EINT2_IRQHandler(void){
	if(octActual>=0){
		octActual--;
			for(int i = 0;i<13;i++){
				notas[i]/=2;
			}
	}
}

void EINT3_IRQHandler(void){
	if(octActual<8){
		octActual++;
			for(int i = 0;i<13;i++){
				notas[i]*=2;
			}
	}
}











