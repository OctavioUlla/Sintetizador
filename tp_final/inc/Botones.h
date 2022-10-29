#include "LPC17xx.h"

#ifndef BOTONES_H_
#define BOTONES_H_

#define CANTIDADSGNLS  ((3))

void prevSgn(uint8_t *sgnActual);
void nextSgn(uint8_t *sgnActual);
void disminuirOct(uint8_t *octActual, uint16_t notas[]);
void aumentarOct(uint8_t *octActual, uint16_t notas[]);

#endif /* BOTONES_H_ */
