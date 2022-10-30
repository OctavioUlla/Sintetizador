#include "LPC17xx.h"
#include "lpc17xx_gpdma.h"
#include "Const.h"

#ifndef CONFIG_H_
#define CONFIG_H_

void cfgPines();
void cfgDAC();
void cfgDMA(uint16_t sgnInicial[],GPDMA_LLI_Type *listaDma);
void makeSignals(uint16_t signals[][TRANSFERSIZE]);


#endif /* CONFIG_H_ */
