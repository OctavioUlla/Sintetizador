/*
 * Config.h
 *
 *  Created on: 28 oct. 2022
 *      Author: ferminverdolini
 */
#include "LPC17xx.h"


#ifndef CONFIG_H_
#define CONFIG_H_

void cfgPines();
void cfgDAC();
void cfgDMA(uint16_t sgnRect[]);
void makeSignals(uint16_t sgnRect[],uint16_t sgnTriang[],uint16_t sgnSierra[]);

#define TRANSFERSIZE  ((256))
#define DACSIZE  ((1024))

#endif /* CONFIG_H_ */
