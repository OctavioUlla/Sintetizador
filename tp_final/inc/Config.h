/*
 * Config.h
 *
 *  Created on: 28 oct. 2022
 *      Author: ferminverdolini
 */
#include "LPC17xx.h"
#include "lpc17xx_gpdma.h"

#ifndef CONFIG_H_
#define CONFIG_H_

void cfgPines();
void cfgDAC();
void cfgDMA(uint16_t sgnRect[],GPDMA_LLI_Type *listaDma);
void makeSignals(uint16_t sgnRect[],uint16_t sgnTriang[],uint16_t sgnSierra[]);

#define TRANSFERSIZE  ((256))
#define DACSIZE  ((1024))

#endif /* CONFIG_H_ */
