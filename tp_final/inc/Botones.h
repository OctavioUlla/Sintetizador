#include "LPC17xx.h"
#include "lpc17xx_gpdma.h"
#include "Const.h"

#ifndef BOTONES_H_
#define BOTONES_H_

void prevSgn(uint8_t *sgnActual,uint32_t signals[][TRANSFERSIZE]);
void nextSgn(uint8_t *sgnActual,uint32_t signals[][TRANSFERSIZE]);
void disminuirOct(uint8_t *octActual, uint16_t notas[]);
void aumentarOct(uint8_t *octActual, uint16_t notas[]);

#endif /* BOTONES_H_ */
