/*
 * config.c
 *
 *  Created on: 28 oct. 2022
 *      Author: ferminverdolini
 */
#include "Config.h"

#include "lpc17xx_dac.h"
#include "lpc17xx_timer.h"
#include "lpc17xx_pinsel.h"
#include "lpc17xx_gpio.h"
#include "lpc17xx_exti.h"
#include "lpc17xx_gpdma.h"
#include <cr_section_macros.h>


// Funcion que configura el DMA
void cfgDAC(){
	DAC_Init(LPC_DAC);

	// Se configura el DAC
	DAC_CONVERTER_CFG_Type daccfg;
	daccfg.CNT_ENA = 1; // Se habilita el timeout counter
	daccfg.DBLBUF_ENA = 0; // Se deshabilita el doble buffereo
	daccfg.DMA_ENA = 1; // Se habilita las request a DMA
	DAC_ConfigDAConverterControl(LPC_DAC,&daccfg);

}

// Funcion que configura el DMA y prend el DMA
void cfgDMA(uint16_t sgnRect[]){
	GPDMA_Init();
	// Se inicializa el DMA apuntando a la lista de la señal rectangula
	GPDMA_LLI_Type lista;
	lista.NextLLI = (uint32_t) &lista;
	lista.SrcAddr = (uint32_t) sgnRect[0];
	lista.DstAddr = (uint32_t) &(LPC_DAC ->DACR);
	lista.Control = TRANSFERSIZE | (1 << 18) | ( 1 << 21);

	GPDMA_Channel_CFG_Type dmacfg;
	dmacfg.ChannelNum = 0;
	dmacfg.TransferSize = TRANSFERSIZE;
	dmacfg.TransferType = GPDMA_TRANSFERTYPE_M2P;
	dmacfg.SrcMemAddr = (uint32_t) lista.SrcAddr;
	dmacfg.DstConn = GPDMA_CONN_DAC;
	dmacfg.DMALLI = (uint32_t) &lista;

	GPDMA_Setup(&dmacfg);
	GPDMA_ChannelCmd(0,DISABLE);
	// Se desactiva la interrupcion para que el dac no comience a pedir datos

	// Se configura el puerto 0.26 como AOUT
		PINSEL_CFG_Type dac;
		dac.Portnum = 0;
		dac.Pinnum = 26;
		dac.Pinmode = 2;
		dac.OpenDrain = PINSEL_PINMODE_NORMAL;
	PINSEL_ConfigPin(&dac); // Se prende el DAC
}
void makeSignals(uint16_t sgnRect[],uint16_t sgnTriang[],uint16_t sgnSierra[]){
	uint16_t i;
	uint16_t pendiente = DACSIZE/TRANSFERSIZE;

	for(i=0;i<TRANSFERSIZE;i++){
		if(i<TRANSFERSIZE/2){
			sgnRect[i]=0;
			sgnTriang[i]=i*pendiente*2;
		}else{
			sgnRect[i]=DACSIZE-1;
			sgnTriang[i]=(TRANSFERSIZE-i-1)*pendiente*2;
		}
		sgnSierra[i]=i*pendiente;
	}
}

