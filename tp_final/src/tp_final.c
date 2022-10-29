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
#include "Botones.h"

// TODO: insert other definitions and declarations here



// Arreglo de arreglos donde se guardan las formas de onda
// Posicion  de las seniales en el enum sgnls en config.h
uint16_t signals[CANTIDADSGNLS][TRANSFERSIZE];
//uint16_t sgnRect[TRANSFERSIZE]; // 0
//uint16_t sgnTriang[TRANSFERSIZE]; // 1
//uint16_t sgnSierra[TRANSFERSIZE]; // 2

// Entero para seleccionar la señal
uint8_t sgnActual = 0;
uint8_t octActual = 4;
// Arreglo de las 13 notas inicializado en la 4ta Octava
uint16_t notas[13]= {262,277,294,311,330,349,370,392,415,440,466,494,523};
// Lista a la que apunta el dma, se crea global para variar la senial
GPDMA_LLI_Type listaDma;

int main(void) {
	makeSignals(signals);
	cfgPines();
	cfgDAC();
	cfgDMA(signals[SGNRECT],&listaDma);

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

void EINT0_IRQHandler(void){
	prevSgn(&sgnActual);
	changeSgn(&sgnActual,&listaDma);
}

void EINT1_IRQHandler(void){
	nextSgn(&sgnActual);
	changeSgn(&sgnActual,&listaDma);
}

void EINT2_IRQHandler(void){
	aumentarOct(&octActual,notas);

}

void EINT3_IRQHandler(void){
	disminuirOct(&octActual, notas);
}











