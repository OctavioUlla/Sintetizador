#include "LPC17xx.h"
#include "lpc17xx_gpdma.h"
#include "Const.h"

#ifndef BOTONES_H_
#define BOTONES_H_

void prevSgn(uint8_t *sgnActual);
void nextSgn(uint8_t *sgnActual);
void disminuirOct(uint8_t *octActual);
void aumentarOct(uint8_t *octActual);

#endif /* BOTONES_H_ */
