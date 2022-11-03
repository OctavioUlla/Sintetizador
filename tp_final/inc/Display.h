#ifndef _DISPLAY_H_
#define _DISPLAY_H_

#include "LPC17xx.h"

#define MODE_100kbps 100000
#define MODE_400kbps 400000
#define MODE_1Mbps 1000000

void DisplayInit(uint32_t frecuencia, uint8_t intPrioridad);
void SendCmd(char cmd);

#endif
